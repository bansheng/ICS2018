#include "proc.h"
#include "memory.h"
#include "fs.h"
#ifdef HAS_VME
#define DEFAULT_ENTRY 0x8048000
#else
#define DEFAULT_ENTRY 0x4000000
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  int file = fs_open(filename,0,0);
  if(file==-1)
    panic("File does not exist!\n");
  #ifdef HAS_VME
  uint32_t progSize = fs_filesz(file);
  uint32_t pgCnt = progSize / (pcb->as.pgsize)+((progSize%(pcb->as.pgsize))!=0);
  /*uint32_t tblCnt = pgCnt / 1024 +((pgCnt%1024)!=0);
  for(int i = 0; i < tblCnt; i++){
    void* npgtbl = new_page(1);
        //printf("Attempted to create page table for VA:%d at PA:%d\n",DEFAULT_ENTRY+(i<<20),
          //(uintptr_t) npgtbl);
    _map(&pcb->as,(void*)(DEFAULT_ENTRY+(i<<22)),npgtbl,1);
  }*/
  for(int i = 0; i < pgCnt; i++){
    void* npg = new_page(1);
    _map(&pcb->as,(void*)(long)(DEFAULT_ENTRY+(i<<12)),npg,0);
  }
  #endif
  
  fs_read(file,(void*)(long)DEFAULT_ENTRY,fs_filesz(file));
  return DEFAULT_ENTRY;

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
