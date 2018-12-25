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
int _map(_Protect *p, void *va, void *pa, int prot) {
	PDE *dir = p->ptr; // 一级页表
	PTE pde = dir[PDX(va)];//二级页表内容
	PTE* pgtab; //取到二级页表
	
	if (prot & PTE_P) {
		pgtab = (PTE *)PTE_ADDR(pde); //取到高20位,即为二级页表的起始位置
		printf("%X %X\n", pde, *pgtab);
		pgtab[PTX(va)] = PTE_ADDR(pa) | prot; //赋值内容
		return 1;
	}
	else
	{
		printf("映射无效\n");
		return 0;
/*		pgtab = (PDE*)(pgalloc_usr(1));*/
/*		for (int i = 0; i < NR_PTE; i ++) {*/
/*		  pgtab[i] = 0;*/
/*		}*/
/*		pde = PTE_ADDR(pgtab) | PTE_P;*/
	}
	
	
/*	if (*pde & PTE_P) {*/
/*		pgtab = (PTE *)PTE_ADDR(*pde);*/
/*	} */
/*	else {*/
/*		pgtab = (PTE *)palloc_f();*/
/*		for (int i = 0; i < NR_PTE; i ++) {*/
/*			pgtab[i] = 0;*/
/*		}*/
/*		*pde = PTE_ADDR(pgtab) | PTE_P;*/
/*	}*/
/*	pgtab[PTX(va)] = PTE_ADDR(pa) | PTE_P;*/
}

_Context *_ucontext(_Protect *p, _Area ustack, _Area kstack, void *entry, void *args) {
	//包括返回地址和main的参数
	_Context* ct = (ustack.end - sizeof(_Context) - 2*sizeof(uintptr_t)); 
	
	// printf("ct = %X  end = %X\n", (uintptr_t)ct, (uintptr_t)stack.end);
	memset(ct, 0, sizeof(_Context));
	ct->prot = p; //设置地址空间
	ct->eip  = (uintptr_t)entry; //设置返回值
	ct->cs = 8;
	*(uintptr_t *)(ustack.end - sizeof(sizeof(uintptr_t))) = 0; //main的参数
	// ct->irq = 0x81; //yeild
	return ct;
}
