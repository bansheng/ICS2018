#include "proc.h"
#include "memory.h"
#include "fs.h"
#define DEFAULT_ENTRY ((void *)0x8048000)

static uintptr_t loader(PCB *pcb, const char *filename) {
	// TODO();
	_Protect *as = &(pcb->as);
	int fd = fs_open(filename, 0, 0);
	size_t nbyte = fs_filesz(fd);
	void *pa;
	void *va;
	void *end = DEFAULT_ENTRY + nbyte;
	
	printf("loaded: [%d]%s size:%d start=%X, end=%X\n", fd, filename, nbyte, DEFAULT_ENTRY, end);
	
	for (va = (void *)DEFAULT_ENTRY; va < end; va += PGSIZE) {
		pa = new_page(1); //new_page(1) 和 _protect 的区别在于_protect里面还是有默认的内核地址映射的
		// _map(_Protect *p, void *va, void *pa, int prot)
		// printf("va = %X => pa = %X\n", va, pa);
		_map(as, va, pa, 1);
		fs_read(fd, pa, (end - va) < PGSIZE ? (end - va) : PGSIZE);
	}
	
/*	int fd = fs_open(filename, 0, 0);*/
/*	fs_read(fd, (void *)DEFAULT_ENTRY, fs_filesz(fd));*/
/*	fs_close(fd);*/
	
	return (uintptr_t)DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
	uintptr_t entry = loader(pcb, filename);
	((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
	_Area stack;
	stack.start = pcb->stack;
	stack.end = stack.start + sizeof(pcb->stack);

	pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
	_protect(&(pcb->as)); //创建默认地址空间
	printf("地址空间%X\n", (uintptr_t)((pcb->as).ptr));
	uintptr_t entry = loader(pcb, filename);
	_Area stack;
	stack.start = pcb->stack;
	stack.end = stack.start + sizeof(pcb->stack);

	pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}