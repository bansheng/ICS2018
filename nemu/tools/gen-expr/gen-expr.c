#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];
static char code_buf[65536];

static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";


static inline int choose(int n){
	return rand()%n;
}

static inline void gen(char e) {
	int i=0;
	while(buf[i] !='\0')
		i++;
	buf[i] = e;
	buf[i+1] = '\0';
//	printf("%c:%s\n", e, buf);
}

static inline void gen_num() {
	uint32_t num;
	
	
	num = ((uint32_t) rand()) % 10000;
	
	switch(choose(2)) {
		case 0: sprintf(buf + strlen(buf), "%d", num); break;
		case 1: sprintf(buf + strlen(buf), "0x%x", num); break;
		default: break;  
	}

	//printf("%s\n",buf);
}

static inline void gen_rand_op() {
	switch(choose(8)) {
		case 0: gen('*'); break;
		case 1: gen('/'); break;
		case 2: gen('+'); break;
		case 3: gen('-'); break;
		case 4: gen('='); gen('='); break;
		case 5: gen('!'); gen('='); break;
		case 6: gen('&'); gen('&'); break;
		case 7: gen('|'); gen('|'); break;
		default : break;
	}

	//printf("%s\n",buf);
} 


static inline void gen_rand_expr() {
	char *s = buf;
	if(strlen(buf) > 0 && *(buf+strlen(buf)-1) == '/'){
		s = buf + strlen(buf);
	}
	int n = choose(3);
	//printf("choose:%d\n",n);
	switch(n) {
		case 0 : gen_num(); break;
		case 1 : gen('('); gen_rand_expr(); gen(')'); break;
		default : gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
	}
	//printf("expr: %s\n",buf);
	
	/*Test division 0 */
	if(s != buf){
		sprintf(code_buf, code_format, s);

		FILE *fp = fopen(".code.c","w");
		assert(fp != NULL);
		fputs(code_buf, fp);
		fclose(fp);
	
		int ret = system("gcc .code.c -o .expr");
		if(ret != 0){
			printf("ret: %d\n",ret);
		}
		
		fp = popen("./.expr", "r");
		assert(fp != NULL);

		int result;
		fscanf(fp, "%d", &result);
		pclose(fp);
		
		if(result == 0){
			memset((void*)s, 0, strlen(s) * sizeof(char));
			gen_rand_expr();
		}
	}
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
	//printf("in main\n");

  for (i = 0; i < loop; i ++) {
	  memset((void *)buf, 0, 65536 * sizeof(char));	
	//	printf("in for\n");
    gen_rand_expr();
		
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen(".code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc .code.c -o .expr");
    if (ret != 0) continue;

    fp = popen("./.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);
	  printf("%u %s\n", result, buf);
  }
  return 0;
}

