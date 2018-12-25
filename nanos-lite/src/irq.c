#include "common.h"
#include "syscall.h"
extern _Context* do_syscall(_Context *c);
extern _Context* schedule(_Context *prev);

static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {
  	case _EVENT_YIELD: 
  		printf("_EVENT_YIELD\n");
  		return schedule(c); //eax里面
/*	case _EVENT_YIELD: printf("_EVENT_YIELD\n"); break;*/
  	case _EVENT_SYSCALL: 
  		printf("_EVENT_SYSCALL\n");
  		do_syscall(c); break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event); //user_handler函数指针赋值
}
