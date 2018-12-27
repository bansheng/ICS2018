#include <x86.h>
#include <klib.h>
// PGSIZE 4096
#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*pgalloc_usr)(size_t);
static void (*pgfree_usr)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) { //init Kernel memory mappings
	pgalloc_usr = pgalloc_f;
	pgfree_usr = pgfree_f;

	int i;

	// make all PDEs invalid
	for (i = 0; i < NR_PDE; i ++) {
		kpdirs[i] = 0;
	}

	PTE *ptab = kptabs;
	for (i = 0; i < NR_KSEG_MAP; i ++) {
		uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
		uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
		for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
			// fill PDE
			kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

			// fill PTE
			PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
			PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
			for (; pte < pte_end; pte += PGSIZE) {
				*ptab = pte;
				ptab ++;
			}
		}
	}

	set_cr3(kpdirs);
	set_cr0(get_cr0() | CR0_PG);

	return 0;
}

int _protect(_Protect *p) {
	PDE *updir = (PDE*)(pgalloc_usr(1));
	p->pgsize = 4096;
	p->ptr = updir; //目录表信息
	// map kernel space
	for (int i = 0; i < NR_PDE; i ++) {
		updir[i] = kpdirs[i];
	}

	p->area.start = (void*)0x8000000;
 	p->area.end = (void*)0xc0000000;
	return 0;
}

void _unprotect(_Protect *p) {
	pgfree_usr(p->ptr);
	p->ptr = p->area.start = p->area.end = NULL;
}

static _Protect *cur_as = NULL;
void get_cur_as(_Context *c) {
  	c->prot = cur_as;
}

void _switch(_Context *c) {
	set_cr3(c->prot->ptr);
	cur_as = c->prot;
}

// 它用于将地址空间p中虚拟地址va所在的虚拟页, 以prot的权限映射到pa所在的物理页. 当prot中的present位为0时, 表示让va的映射无效.
int _map(_Protect *p, void *va, void *pa, int mode) {
  // uint32_t index = ((uint32_t)va >> 22) & 0x3ff;
  // PDE *pdir = (PDE*)((uint32_t*)(p->ptr))[index];
  // // alloc a new page
  // if (!((uint32_t)pdir & 0x1)) {
  //   pdir = (PDE*)(pgalloc_usr(1));
  //   ((uint32_t*)(p->ptr))[index] = (uintptr_t)pdir | PTE_P;
    
  //   for (int i = 0; i < NR_PTE; i ++)
  //     pdir[i] = 0;
  // }
  // PTE *ptab = (PTE*)PTE_ADDR(pdir);
  // ptab[((uint32_t )va >> 12) & 0x3ff] = ((uintptr_t)pa & ~0xfff) | PTE_P;
  // return 0;
  uint32_t* ptr = (uint32_t*)p->ptr;
  uint32_t shift = (uintptr_t)va >> 22;
  uintptr_t tr = ptr[shift];
  if (tr == kpdirs[shift]) 
  {
      PTE *uptable = (PTE*)(pgalloc_usr(1));
      ptr[shift] = (uintptr_t)uptable | PTE_P;
  }
  tr = ptr[shift];
  shift = (((uintptr_t)va) & 0x003ff000) >> 12;
  uint32_t* pgr = (uint32_t*)(tr & 0xfffff000);
  pgr[shift] = (uintptr_t)pa | PTE_P;
  return 0;
}

_Context *_ucontext(_Protect *p, _Area ustack, _Area kstack, void *entry, void *args) {
	//包括返回地址和main的参数
	_Context* ct = (ustack.end - sizeof(_Context) - 4*sizeof(uintptr_t)); 
	
	// printf("ct = %X  end = %X\n", (uintptr_t)ct, (uintptr_t)stack.end);
	memset(ct, 0, sizeof(_Context) + 4*sizeof(uintptr_t));
	ct->prot = p; //设置地址空间
	ct->eip  = (uintptr_t)entry; //设置返回值
	ct->cs = 8;
	ct->eflags = 0x2 | (1 << 9);  /* pre-set value | eflags.IF */
	*(uintptr_t *)(ustack.end - sizeof(sizeof(uintptr_t))) = 0; //main的参数
	// ct->irq = 0x81; //yeild
	return ct;
}
