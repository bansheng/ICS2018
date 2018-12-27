#include "proc.h"
#include "fs.h"

#ifdef HAS_VME
#define DEFAULT_ENTRY 0x8048000
#else
#define DEFAULT_ENTRY 0x4000000
#endif
//void _switch_addr(_Protect *p);
void _switch(_Context *c);
void get_cur_as(_Context *c);
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
  _Context fake;
  fake.prot = &pcb->as;
  _switch(&fake);
  #endif
  
  fs_read(file,(void*)(long)DEFAULT_ENTRY,fs_filesz(file));
  return DEFAULT_ENTRY;

}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
#ifdef HAS_VME
  _protect(&(pcb->as));
#endif
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
  pcb->cp->prot = &(pcb->as);
}

#ifdef HAS_VME
  static _Context old;
  static _Protect m;
#endif

void context_uload(PCB *pcb, const char *filename) {
  #ifdef HAS_VME
  get_cur_as(&old);
  if(old.prot==NULL)
    {
      _protect(&(pcb->as));
      m = pcb->as;
    }
  else
    {
      m = *(old.prot);
      _protect(&(pcb->as));
    }
  #endif
  uintptr_t entry = loader(pcb, filename);
  old.prot = &m;
  _switch(&old);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
  int file = fs_open(filename,0,0);
  pcb->cur_brk = DEFAULT_ENTRY+fs_filesz(file);
  pcb->max_brk = pcb->cur_brk;
}
