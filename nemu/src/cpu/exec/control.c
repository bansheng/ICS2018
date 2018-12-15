#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(jmp) {
	// the target address is calculated at the decode stage
	rtl_j(decoding.jmp_eip);

	print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
	// the target address is calculated at the decode stage
	uint32_t cc = decoding.opcode & 0xf;
	rtl_setcc(&t0, cc);
	rtl_li(&t1, 0);
	rtl_jrelop(RELOP_NE, &t0, &t1, decoding.jmp_eip);

	print_asm("j%s %x", get_cc_name(cc), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1;

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  rtl_push(&decoding.seq_eip);
  decoding.is_jmp = 1;

  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  decoding.is_jmp = 1;
  rtl_pop(&id_dest->val);
  decoding.jmp_eip = id_dest->val;

  print_asm("ret");
}

make_EHelper(call_rm) {
  decoding.is_jmp = 1;
  decoding.jmp_eip = id_dest->val;
  rtl_push(&decoding.seq_eip);

  print_asm("call *%s", id_dest->str);
}
