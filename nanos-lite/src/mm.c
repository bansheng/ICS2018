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
  //printf("Current break:%d\tNew break:%d\n",current->cur_brk,new_brk);
  #ifdef HAS_VME
  if(new_brk < current->max_brk)
  {
    current->cur_brk = new_brk;
    return 0;
  }
  else{
    if((0xfffff&(new_brk>>12))>(0xfffff&((current->max_brk)>>12))){
      /*printf("Address Maximum break at %d, but a break at %d is needed\n",
          current->max_brk,new_brk);
      printf("Maximum break at %d, but a break at %d is needed\n",
          0xfffff&(current->max_brk)>>12,0xfffff&(new_brk>>12));*/
    uint32_t npgCnt = (0xfffff&(new_brk>>12)) - (0xfffff&((current->max_brk)>>12));
    //printf("Brk CR3:%d\n",fake.prot->ptr);
    for(uint32_t i = 0; i < npgCnt; i++){
     // printf("Assigning page for virtual address %d in membrk\n",current->max_brk+(i+1)*PGSIZE);
      _map(&current->as,(void*)(current->max_brk+(i+1)*PGSIZE),new_page(1),0);
    }}
    current->max_brk = new_brk;
    current->cur_brk = new_brk;
  }
  #endif
  return 0;
}

void init_mm() {
	pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
	Log("free physical pages starting from %p", pf);
	_vme_init(new_page, free_page);
}
