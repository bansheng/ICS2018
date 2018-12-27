#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;

void switch_boot_pcb() {
	current = &pcb_boot;
}

void hello_fun(void *arg) {
	int j = 1;
	while (1) {
		Log("Hello World from Nanos-lite for the %dth time!", j);
		j ++;
		_yield();
	}
}

void init_proc() {
	pcb_boot = pcb[0];

/*	*/
/*	extern void context_kload(PCB *pcb, void *entry);*/
/*	context_kload(&pcb[0], (void *)hello_fun);*/
/*	*/
	extern void context_uload(PCB *pcb, const char *filename);
	context_uload(&pcb[0], "/bin/text");
	// context_uload(&pcb[1], "/bin/hello");
/*	*/
 	switch_boot_pcb();
/* 	printf("init proc over\n");*/
}

_Context* schedule(_Context *prev) {
	// save the context pointer
	current->cp = prev;

	// always select pcb[0] as the new process
    current = &pcb[0];
	// current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

	// then return the new context
	return current->cp;
}
