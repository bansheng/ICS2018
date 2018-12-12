#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.  
* Type 'man regex' for more information about POSIX regex functions.  
*/
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, 
	EQ , NEQ , AND , OR , MINUS , POINTOR , NUMBER , HEX , REGISTER , MARK
	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
	int priority;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	{"\\b[0-9]+\\b",NUMBER,0},			// number
	{"\\b0[xX][0-9a-fA-F]+\\b",HEX,0},//hex
	{"\\$[e|E][ax|AX|bx|BX|cx|CX|dx|DX|bp|BP|sp|SP|si|SI|di|DI]",REGISTER,0},		// register
	{"\\$[A|B|C|D|a|b|c|d][h|H|l|H|x|X]",REGISTER,0},		// register
	{"\\b[a-zA-Z_0-9]+" , MARK, 0},		// mark
	{"!=",NEQ,3},						// not equal	
	{"!",'!',6},						// not
	{"\\*",'*',5},						// mul
	{"/",'/',5},						// div
	{"\\t+",NOTYPE,0},					// tabs
	{" +",NOTYPE,0},					// spaces
	{"\\+",'+',4},						// plus
	{"-",'-',4},						// sub
	{"==", EQ,3},						// equal
	{"&&",AND,2},						// and
	{"\\|\\|",OR,1},					// or
	{"\\(",'(',7},                      // left bracket   
	{"\\)",')',7},                      // right bracket 
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() { // 将正则表达式进行编译,写入re数组
	int i;
	char error_msg[128];
	int ret;
	for(i = 0; i < NR_REGEX; i ++) { 
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED); 
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128); //解释错误信息
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

/* str成员就是用来做这件事情的
 * 需要注意的是, str成员的长度是有限的, 当你发现缓冲区将要溢出的时候, 要进行相应的处理
 */
typedef struct tokens {
	int type;
	char str[32];
	int priority;
} Tokens;

Tokens token[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch; //存放匹配开始位置和结束位置
	nr_token = 0;
	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) { //必须从开始完成匹配
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				char *tmp = e + position + 1;
				int substr_len = pmatch.rm_eo; // eo位置是不匹配字符
				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */
				switch(rules[i].token_type) {
					case NOTYPE: break;
					case REGISTER: //寄存器把那个特殊的头部去掉
						token[nr_token].type = rules[i].token_type;
						token[nr_token].priority = rules[i].priority; 
						strncpy (token[nr_token].str,tmp,substr_len-1);
						token [nr_token].str[substr_len-1]='\0';
						nr_token ++;
						break; 
					default:
						token[nr_token].type = rules[i].token_type;
						token[nr_token].priority = rules[i].priority;
						strncpy (token[nr_token].str,substr_start,substr_len);
						token[nr_token].str[substr_len]='\0';
						nr_token ++;
						break;
				}
				position += substr_len;
				break;
 			}
 		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
 	}
	return true; 
}

bool check_parentheses (int l,int r)
{
	int i;
	if (token[l].type == '(' && token[r].type ==')')
	{
		int lc = 0, rc = 0;
		for (i = l + 1; i < r; i ++)
		{
			if (token[i].type == '(')lc ++;
			if (token[i].type == ')')rc ++;
			if (rc > lc) return false;	//右括号数目不能超过左边括号数目
		}
		if (lc == rc) return true;
	}
	return false;
}

/*
 *寻找 token[l] 到 token[r] 间的主运算符
 *出现在一对括号中的token不是主运算符. 注意到这里不会出现有括号包围整个表达式的情况, 因为这种情况已经在check_parentheses()相应的if块中被处理了.
 *主运算符的优先级在表达式中是最低的. 这是因为主运算符是最后一步才进行的运算符.
 *当有多个运算符的优先级都是最低时, 根据结合性, 最后被结合的运算符才是主运算符. 一个例子是1 + 2 + 3, 它的主运算符应该是右边的+.
 */

int dominant_operator (int l,int r)
{
	int i;
	int min_priority = 10;
	int oper = l;
	int cnt = 0;
	for (i = l; i <= r;i ++)
	{
		if (token[i].type == NUMBER || token[i].type == HEX || token[i].type == REGISTER || token[i].type == MARK)
			continue;
		
		if (token[i].type == '(') cnt++;
		if (token[i].type == ')') cnt--;
		if (cnt != 0) continue; //左边还有括号,还是处于括号之中
		
		if (token[i].priority <= min_priority) {
			min_priority=token[i].priority; oper=i;
		}
 	}
	return oper;
}

uint32_t eval(int l,int r, bool *legal) {
	if (!(*legal)) return -1;
	if (l > r) {Assert (l>r,"表达式计算错误未知!\n");return -1;}
	if (l == r) 
	{
		uint32_t num = 0;
		if (token[l].type == NUMBER)
			sscanf(token[l].str,"%d",&num);
		else if (token[l].type == HEX)
			sscanf(token[l].str,"%x",&num);
		else if (token[l].type == REGISTER)
		{
			if (strlen (token[l].str) == 3) {
				int i;
				for (i = R_EAX; i <= R_EDI; i ++)
					if (strcmp (token[l].str,regsl[i]) == 0) break;
				if (i > R_EDI)
					if (strcmp (token[l].str,"eip") == 0)
						num = cpu.eip;
					else Assert (1,"no this register!\n");
				else num = reg_l(i);
			}
			else if (strlen (token[l].str) == 2) {
				if (token[l].str[1] == 'x' || token[l].str[1] == 'p' || token[l].str[1] == 'i') {
					int i;
					for (i = R_AX; i <= R_DI; i ++)
						if (strcmp (token[l].str,regsw[i]) == 0)break;
					num = reg_w(i);
				}
				else if (token[l].str[1] == 'l' || token[l].str[1] == 'h') {
					int i;
					for (i = R_AL; i <= R_BH; i ++)
						if (strcmp (token[l].str,regsb[i]) == 0)break;
					num = reg_b(i);
				}
				else assert (1);
			}
		}
		// 这个部分用于查找符号表中的变量的值
/*		if (token[l].type == MARK)*/
/*		{*/
/*			int i;*/
/*			for (i=0;i<nr_symtab_entry;i++)*/
/*			{*/
/*				if ((symtab[i].st_info&0xf) == STT_OBJECT)*/
/*				{*/
/*					char tmp [32];*/
/*					int tmplen = symtab[i+1].st_name - symtab[i].st_name - 1;*/
/*					strncpy (tmp,strtab+symtab[i].st_name,tmplen);*/
/*					tmp [tmplen] = '\0';*/
/*					if (strcmp (tmp,token[l].str) == 0)*/
/*					{*/
/*						num = symtab[i].st_value;*/
/*					}*/
/*				}*/
/*			}*/
/*		}*/
		else
		{
			//printf("type = %d\n", token[l].type);
			*legal = false;
			return -1;
		}
		return num;
	}
	else if (check_parentheses (l,r) == true) return eval(l + 1, r - 1, legal);
 	else {
		int op = dominant_operator (l,r);
 		if (l == op || token [op].type == POINTOR || token [op].type == MINUS || token [op].type == '!')
		{
			uint32_t val = eval (l + 1, r, legal);
			switch (token[l].type)
 			{
				//case POINTOR:current_sreg = R_DS;return swaddr_read (val,4);
				case MINUS:return -val;
				case '!':return !val;
				default: 
					//printf("不合法表达式\n"); //有些主运算符无法处于第一位
					*legal = false;
					return -1;
			} 
		}
		uint32_t val1 = eval (l, op - 1, legal);
		uint32_t val2 = eval (op + 1, r, legal);
		switch (token[op].type)
		{
			case '+':return val1 + val2;
			case '-':return val1 - val2;
			case '*':return val1 * val2;
			case '/':return val1 / val2;
			case EQ:return val1 == val2;
			case NEQ:return val1 != val2;
			case AND:return val1 && val2;
			case OR:return val1 || val2;
			default:
				//printf("不合法表达式\n"); //有些主运算符无法处于第一位
				*legal = false;
				return -1;
  		}
  	}
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
  	}
	int i;
	for (i = 0;i < nr_token; i ++) { //识别负数和指针
		//printf("%d\n", token[i].type == MARK);
 		if (token[i].type == '*' && (i == 0 || (token[i - 1].type != NUMBER && token[i - 1].type != HEX && token[i - 1].type != REGISTER && token[i - 1].type != MARK && token[i - 1].type !=')'))) {
			token[i].type = POINTOR;
			token[i].priority = 6;
		}
		if (token[i].type == '-' && (i == 0 || (token[i - 1].type != NUMBER && token[i - 1].type != HEX && token[i - 1].type != REGISTER && token[i - 1].type != MARK && token[i - 1].type !=')'))) {
			token[i].type = MINUS;
			token[i].priority = 6;
 		}
  	}
	/* TODO: Insert codes to evaluate the expression. */	
	*success = true;
	printf("success = %d\n", *success);
	return eval(0, nr_token-1, success);
}


