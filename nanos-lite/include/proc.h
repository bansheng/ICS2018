#ifndef __PROC_H__
#define __PROC_H__

#include "common.h"
#include "memory.h"

#define STACK_SIZE (8 * PGSIZE)

typedef union {
  uint8_t stack[STACK_SIZE] PG_ALIGN; //32KB 32K个字节
  struct {
    _Context *cp;
    _Protect as;
    uintptr_t cur_brk;
    // we do not free memory, so use `max_brk' to determine when to call _map()
    uintptr_t max_brk;
  };
} PCB;

extern PCB *current;
void context_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void *entry);
void naive_uload(PCB *pcb, const char *filename);

#endif
