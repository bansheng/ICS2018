#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
	// TODO();
	if(id_dest->width == 1){
	  	uint8_t utmp = id_dest->val;
		int8_t temp = utmp;
		id_dest->val = temp;
	}
   	rtl_push(&id_dest->val);  

	print_asm_template1(push);
}

make_EHelper(pop) {
	// TODO();
	rtl_pop(&at);
	if(id_dest->width == 1){
		uint8_t utemp = at;
		int8_t temp = utemp; 
		id_dest->val = temp;
	}
	else 
	  	id_dest->val = at;
	
	print_asm_template1(pop);
}

make_EHelper(pusha) {
	TODO();

	print_asm("pusha");
}

make_EHelper(popa) {
	TODO();

	print_asm("popa");
}

make_EHelper(leave) {
	// TODO();
	reg_l(4) = reg_l(5);
	rtl_pop(&at);
	reg_w(5) = at;
	
	print_asm("leave");
}

make_EHelper(cltd) {
	if (decoding.is_operand_size_16) {
		TODO();
	}
	else {
		TODO();
	}

	print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t0, &id_src->val, id_src->width);
  operand_write(id_dest, &t0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
