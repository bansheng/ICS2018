#include <am.h>
#include <x86.h>
#include <klib.h>

#define STACK_SIZE 8 * 4096

static _Context* (*user_handler)(_Event, _Context*) = NULL;

void vecsys();
void vectrap();
void vecnull();

_Context* irq_handle(_Context *tf) {
	_Context *next = tf;
/*	printf("cpu eax:0x%08x\n", tf->eax);*/
/*    printf("cpu ecx:0x%08x\n", tf->ecx);*/
/*    printf("cpu edx:0x%08x\n", tf->edx);*/
/*    printf("cpu ebx:0x%08x\n", tf->ebx);*/
/*    printf("cpu esp:0x%08x\n", tf->esp);*/
/*    printf("cpu ebp:0x%08x\n", tf->ebp);*/
/*    printf("cpu esi:0x%08x\n", tf->esi);*/
/*    printf("cpu edi:0x%08x\n", tf->edi);*/
/*    printf("cpu eip:0x%08x\n", tf->eip);*/
	// printf("%d\n", tf->irq);
	if (user_handler) {
		_Event ev = {0};
		switch (tf->irq) {
			case 0x80: ev.event = _EVENT_SYSCALL; break;
			case 0x81: ev.event = _EVENT_YIELD; break;
			default: ev.event = _EVENT_ERROR; break;
		}

		next = user_handler(ev, tf); //进程切换的依赖
		if (next == NULL) {
		  next = tf;
		}
	}

	return next;
}

static GateDesc idt[NR_IRQ];

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i] = GATE(STS_TG32, KSEL(SEG_KCODE), vecnull, DPL_KERN);
  }

  // -------------------- system call --------------------------
  idt[0x80] = GATE(STS_TG32, KSEL(SEG_KCODE), vecsys, DPL_KERN);
  idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), vectrap, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
	// entry本身是函数指针的值
/*	struct _Context {*/
/*	struct _Protect *prot;*/
/*	uintptr_t edi, esi, ebp, esp, ebx, edx, ecx, eax;*/
/*	int       irq;*/
/*	uintptr_t error_code, eip, cs, eflags; */
/*	};*/
/*	_Context* ct = (_Context*)((uintptr_t)stack + STACK_SIZE - sizeof(_Context));*/
	_Context* ct = (stack.end - sizeof(_Context));
	// printf("ct = %X  end = %X\n", (uintptr_t)ct, (uintptr_t)stack.end);
	memset(ct, 0, sizeof(_Context));
	ct->eip = (uintptr_t)entry; //设置返回值
	ct->cs = 8;
	// ct->irq = 0x81; //yeild
	return ct;
}

void _yield() {
  asm volatile("int $0x81");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}
