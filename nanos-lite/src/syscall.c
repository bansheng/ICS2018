#include "common.h"
#include "syscall.h"

uintptr_t sys_write(int fd, const void *buf, size_t count) {
	Log(" 1 ");
	uintptr_t i = 0;
	if (fd == 1 || fd == 2) {
		for(; count > 0; count--) {
			_putc(((char*)buf)[i]);
			i++;;
		}
	}
	return i;
}

_Context* do_syscall(_Context *c) {
	uintptr_t a[4], result = -1;
	a[0] = c->GPR1;
	a[1] = c->GPR2;
	a[2] = c->GPR3;
	a[3] = c->GPR4;
	
	switch (a[0]) {
		case SYS_yield: result = 1; break;
		case SYS_exit: _halt(a[0]); break;
		case SYS_write:
			result = sys_write(a[1], (void *)a[2], a[3]);
			break;
		case SYS_brk:
			result = 0;
			break;
		default: panic("Unhandled syscall ID = %d", a[0]);
	}
	c->GPR1 = result;

	return NULL;
}
