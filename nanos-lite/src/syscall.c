#include "common.h"
#include "syscall.h"
#include "fs.h"
#include "proc.h"

extern void naive_uload(PCB *pcb, const char *filename);

_Context* do_syscall(_Context *c) {
	uintptr_t a[4], result = -1;
	a[0] = c->GPR1;
	a[1] = c->GPR2;
	a[2] = c->GPR3;
	a[3] = c->GPR4;
	
	switch (a[0]) {
		case SYS_yield: 
			result = 1;
			printf("SYS_yield\n");
			break;
		case SYS_exit: 
/*			naive_uload(NULL, "/bin/init"); */
/*			printf("SYS_exit\n");*/
			_halt(0);
			break;
		case SYS_open:
			printf("SYS_open\n");
			result = fs_open((void *)a[1], a[2], a[3]);
			break;
		case SYS_write:
			printf("SYS_write\n");
			result = fs_write(a[1], (void *)a[2], a[3]);
			break;
		case SYS_read:
			printf("SYS_read\n");
			result = fs_read(a[1], (void *)a[2], a[3]);
			break;
		case SYS_lseek:
			printf("SYS_lseek\n");
			result = fs_lseek(a[1], a[2], a[3]);
			break;
		case SYS_close:
			result = fs_close(a[1]);
			break;
		case SYS_brk:
			printf("SYS_brk\n");
			result = mm_brk(a[1]);
			break;
		case SYS_execve:
			// Log("execve %s", a[1]);
			naive_uload(NULL, (void *)a[1]); 
			break;
		default: panic("Unhandled syscall ID = %d", a[0]);
	}
	c->GPRx = result;

	return NULL;
}
