#include "proc.h"
#include "memory.h"

static void *pf = NULL;

void* new_page(size_t nr_page) {
	void *p = pf;
	pf += PGSIZE * nr_page;
	assert(pf < (void *)_heap.end);
	return p;
}

void free_page(void *p) {
	panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t new_brk) {
	printf("current->cur_brk = %X\n", current->cur_brk);
	if (current->cur_brk == 0) {
		current->cur_brk = current->max_brk = new_brk;
	} 
	else {
		if (new_brk > current->max_brk) {
			uintptr_t pa, va;
			for (va = (current->max_brk+0xfff) & ~0xfff; va < new_brk; va += PGSIZE) {
				pa = (uintptr_t)new_page(1);
				_map(&current->as, (void *)va, (void *)pa, 1);
			}
			current->max_brk = va;
		}
		current->cur_brk = new_brk;
	}
	printf("current->cur_brk = %X\n", current->cur_brk);
	return 0;
}

void init_mm() {
	pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
	Log("free physical pages starting from %p", pf);
	_vme_init(new_page, free_page);
}
