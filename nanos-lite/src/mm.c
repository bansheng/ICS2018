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
  // Log("new_brk = %x max_brk = %x\n", new_brk, current->max_brk);
  if(new_brk > current->max_brk){
    uintptr_t va = (current->max_brk & 0xfffff000);
    while(va < (uint32_t)(new_brk & 0xfffff000)){
      void* pa = new_page(1);
      _map(&current->as, (void*)va, pa, 0);
      va += PGSIZE;
      // Log("brk %x, %x\n", va, pa);
    }
    // current->max_brk = (uintptr_t)va;
    current->max_brk = new_brk;
  }
  current->cur_brk = new_brk;
  return 0;
}

void init_mm() {
	pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
	Log("free physical pages starting from %p", pf);
	_vme_init(new_page, free_page);
}
