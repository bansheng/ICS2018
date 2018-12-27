#include "cpu/exec.h"
#include "device/port-io.h"

void difftest_skip_ref();
void difftest_skip_dut();

make_EHelper(lidt) {
	// TODO();
	cpu.idtr.limit = vaddr_read(id_dest->addr, 2);
	if (decoding.is_operand_size_16) {
		cpu.idtr.base = vaddr_read(id_dest->addr + 2, 4) & 0x00ffffff;
	}
	else {
		cpu.idtr.base = vaddr_read(id_dest->addr + 2, 4);	
	}

	print_asm_template1(lidt);
}

make_EHelper(stos){
  if(id_dest->width == 4){
    t0 = vaddr_read(cpu.es+cpu.edi, 4);
    rtl_lr(&cpu.eax, t0, 4);
    if(cpu.eflags.DF == 0){
      rtl_addi(&cpu.edi, &cpu.edi, 4);
    }
    else{
      rtl_subi(&cpu.edi, &cpu.edi, 4);
    }
  }
  
  else if (id_dest->width == 2)
  {
    t0 = vaddr_read(cpu.es+reg_w(R_DI), 2);
    rtl_lr(&cpu.eax, t0, 2);
    if(cpu.eflags.DF == 0){
      rtl_addi(&cpu.edi, &cpu.edi, 2);
    }
    else{
      rtl_subi(&cpu.edi, &cpu.edi, 2);
    }
  }
  else{
    t0 = vaddr_read(cpu.es+reg_w(R_DI), 1);
    rtl_lr(&cpu.eax, t0, 1);
    if(cpu.eflags.DF == 0){
      rtl_addi(&cpu.edi, &cpu.edi, 1);
    }
    else{
      rtl_subi(&cpu.edi, &cpu.edi, 1);
    }
  }
  print_asm("stos");
}

make_EHelper(mov_r2cr) {
	switch (id_dest->reg) {
		case 0: cpu.cr0.val = id_src->val; break;
		case 3: cpu.cr3.val = id_src->val; break;
		default: Assert(0, "unsupported control register");
	}
	
	print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
	switch (id_src->reg) {
		case 0: t0 = cpu.cr0.val; break;
		case 3: t0 = cpu.cr3.val; break;
		default: Assert(0, "unsupported control register");
	}
	operand_write(id_dest, &t0);

	print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#if defined(DIFF_TEST)
	difftest_skip_ref();
#endif
}

make_EHelper(int) {
	// TODO();
	// printf("irq %d\n", id_dest->val);
	raise_intr(id_dest->val, decoding.seq_eip);

	print_asm("int %s", id_dest->str);

#if defined(DIFF_TEST) && defined(DIFF_TEST_QEMU)
	difftest_skip_dut();
#endif
}

make_EHelper(iret) {
	// TODO();
	
	rtl_pop(&decoding.jmp_eip);
	rtl_j(decoding.jmp_eip);
	
	rtl_pop(&t0);
	cpu.cs = t0 & 0xffff;
	rtl_pop(&cpu.flags);
	
	print_asm("iret");
}

make_EHelper(in) {
	// TODO();
	switch(id_src->width)
	{
		case 1: t0 = pio_read_b(id_src->val); break;
		case 2: t0 = pio_read_w(id_src->val); break;
		case 4: t0 = pio_read_l(id_src->val); break;
	}
	operand_write(id_dest, &t0);
	print_asm_template2(in);

#if defined(DIFF_TEST)
	difftest_skip_ref();
#endif
}

make_EHelper(out) {
	// TODO();
	switch(id_src->width)
	{
		case 1: pio_write_b(id_dest->val, id_src->val); break;
		case 2: pio_write_w(id_dest->val, id_src->val); break;
		case 4: pio_write_l(id_dest->val, id_src->val); break;
	}
	print_asm_template2(out);

#if defined(DIFF_TEST)
	difftest_skip_ref();
#endif
}
