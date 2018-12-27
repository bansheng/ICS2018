#include "proc.h"
#include "memory.h"
#include "fs.h"

// #define DEFAULT_ENTRY 0x4000000
#define DEFAULT_ENTRY 0x8048000


static uintptr_t loader(PCB *pcb, const char *filename) {
  // size_t size = get_ramdisk_size();
  // int* addr = (int *)DEFAULT_ENTRY;
  // // ramdisk_read(addr, 0, size);
  // // return DEFAULT_ENTRY;
  // int fd = fs_open(filename, 0, 0);
  // if(fd != -1){
  //   fs_read(fd, addr, fs_filesz(fd));
  // }
  // return DEFAULT_ENTRY;
  int fd = fs_open(filename, 0, 0);
  size_t size = fs_filesz(fd);
  size_t page_cnt = (size + PGSIZE - 1) / PGSIZE;
  void* pa;
  void* va = (void*)DEFAULT_ENTRY;
  for(int i = 0; i < page_cnt; i ++){
    pa = new_page(1);
    _map(&pcb->as, va, pa, 0);
    fs_read(fd, pa, (((size - i * PGSIZE) < PGSIZE) ? (size - i * PGSIZE) : PGSIZE));
    va += PGSIZE;
  }
  pcb->max_brk = (uintptr_t) va;
  pcb->cur_brk = (uintptr_t) va;
  fs_close(fd);
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