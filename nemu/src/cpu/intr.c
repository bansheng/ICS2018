#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

	// TODO();
	union {
		GateDesc gd;
		struct { uint32_t lo, hi; };
	} item;
	vaddr_t addr;

	addr = 8 * NO + cpu.idtr.base;
	item.lo = vaddr_read(addr, 4);
	item.hi = vaddr_read(addr + 4, 4);

	
	t1 = (item.gd.offset_15_0 & 0xFFFF)
	| ((item.gd.offset_31_16 & 0xFFFF) << 16);
	rtl_j(t1);
	
	// 保存eflags cs 返回地址
	rtl_push(&cpu.flags);
	t0 = cpu.cs;
	rtl_push(&t0);
	rtl_push(&ret_addr);
	
	cpu.eflags.IF = 0;
}

void dev_raise_intr() {
}
