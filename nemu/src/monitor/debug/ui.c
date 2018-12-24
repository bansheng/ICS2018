#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
// modify 'readline' function to adjust the exercise.
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

// 单步执行
static int cmd_si(char *args) {
	// 先处理无参数
	 if (args == NULL) {
		cpu_exec(1);
		return 0;
	}
	int n = 0;
	if (sscanf(args, "%d", &n) == EOF) {
		printf("Please input the right argment!\n");
	}	
	else
	{ 
		cpu_exec(n);
	}
	return 0;
}

// 寄存器位数的定义
// enum typeOfRegster { type_8, type_16, type_32, type_eip };

// 打印寄存器内容的子函数
static void print_regster(int size, int index) {
	switch(size) {
		case 1: printf("%%%s\t%#04X\t\t%d\n", reg_name(index, 1), reg_b(index), reg_b(index)); break;
		case 2: printf("%%%s\t%#06X\t\t%d\n", reg_name(index, 2), reg_w(index), reg_w(index)); break;
		case 4: printf("%%%s\t%#010X\t%d\n", reg_name(index, 4), reg_l(index), reg_l(index)); break;
		case -1: printf("%%eip\t%#010X\t%d\n", cpu.eip, cpu.eip); break;
	}
	
}

// 打印程序状态
static int cmd_info(char *args) {
	if(args == NULL)
	{ 
		printf("Unknown Type to print\n");
		return 0;
	}
	switch(*args)
	{ 
		case 'r':
		case 'R':
			for(int i=0; i<8; i++)
				//print_regster(type_32, i);
				print_regster(4, i);
			for(int i=0; i<8; i++)
				//print_regster(type_16, i);
				print_regster(2, i);
			for(int i=0; i<8; i++)
				//print_regster(type_8, i);
				print_regster(1, i);
			print_regster(-1, 0);
			break;
		case 'w':
		case 'W':
			print_wp();
			break;
		default:
			break;
	}
	return 0;
}

// 扫描内存
#define MAX_EXPR_LENGTH 128
static int cmd_scan(char *str) {
	if(str == NULL)
	{
		printf("Unknown size and address to scan\n");
		return 0;
	}
	
	char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *Size = strtok(str, " ");
    int size = -1;
    if (sscanf(Size, "%d", &size) == EOF || size<0) {
		printf("Please input the \"Right\" size to scan\n");
		return 0;
	}
	
    char *args = Size + strlen(Size) + 1;
    if (args >= str_end) { 
      	printf("Please input the \"Right\" address to scan\n");
      	return 0;
    }
	
	bool success = true;
	//printf("%s\n", args);
	uint32_t result = expr(args, &success);
	if(!success) printf("Please input the \"Right\" address to scan\n");
	else
	{
		for(int i=0; i<size; i++)
			printf("%#010X\t", vaddr_read(result + i * 4, 4));
		printf("\n");
	}
	return 0;
}

// 计算表达式
static int cmd_p(char *args) {
	if(args == NULL)
	{
		printf("Please Input the expression to caculate\n");
		return 0;
	}
	bool success = true;
	int result = expr(args, &success);
	if(!success) printf("不合法表达式\n"); //有些主运算符无法处于第一位
	else printf("%#10x\t%d\n",result, result);
	return 0;
}

// 设置监视点
static int cmd_setWP(char *args) {
	new_wp(args);
	return 0;
}

// 删除监视点
static int cmd_delWP(char *args) {
	int n = 0;
	if (sscanf(args, "%d", &n) == EOF) {
		printf("Please input the right argment!\n");
	}	
	else
	{ 
		del_wp(n);
	}
	return 0;
}

// define the cmd_table 命令表
static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "Single instruction execution", cmd_si },
  { "info", "Print the regsters or watchpoints", cmd_info },
  { "x", "Scan the memory to find the value", cmd_scan }, //扫描内存
  { "p", "Caculate the Expression", cmd_p }, //计算表达式
  { "w", "Set the watchpoint", cmd_setWP }, //设置监视点
  { "d", "Delete the watchpoint", cmd_delWP }, //删除监视点
};
// 定义最大支持的命令表下标
#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if (arg == NULL) {
		/* no argument given */
		for (i = 0; i < NR_CMD; i ++) { 
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	return 0;
}

void ui_mainloop(int is_batch_mode) {
	// 定义调试
	// is_batch_mode = 0;
	if (is_batch_mode) { 
		cmd_c(NULL);
		return;
	}

	for (char *str; (str = rl_gets())  != NULL; ) {
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if (cmd == NULL) { continue; }

		/* treat the remaining string as the  arguments,
		* which may need further parsing
		*/
		char *args = cmd + strlen(cmd) + 1;
		if (args >= str_end) { 
			args = NULL;
		}

		#ifdef HAS_IOE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
		#endif

		int i;
		for (i = 0; i < NR_CMD; i ++) { 
			if (strcmp(cmd, cmd_table[i].name) == 0 ) {
				if (cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
