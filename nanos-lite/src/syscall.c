#include "common.h"
#include "syscall.h"
#include "fs.h"

_Context* do_syscall(_Context *c) {
	uintptr_t a[4], result = -1;
	a[0] = c->GPR1;
	a[1] = c->GPR2;
	a[2] = c->GPR3;
	a[3] = c->GPR4;
	
	switch (a[0]) {
		case SYS_yield: 
			result = 1; 
			break;
		case SYS_exit: 
			_halt(a[0]); 
			break;
		case SYS_open:
			result = fs_open((void *)a[1], a[2], a[3]);
			break;
		case SYS_write:
			result = fs_write(a[1], (void *)a[2], a[3]);
			break;
		case SYS_read:
			result = fs_read(a[1], (void *)a[2], a[3]);
			break;
		case SYS_lseek:
			result = fs_lseek(a[1], a[2], a[3]);
			break;
		case SYS_close:
			result = fs_close(a[1]);
			break;
		case SYS_brk:
			// Log("SYS_brk\n");
			result = 0;
			break;
			
		default: panic("Unhandled syscall ID = %d", a[0]);
	}
	c->GPR1 = result;

	return NULL;
}
