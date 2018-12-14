#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

// 01 
make_EHelper(call);
// 02
make_EHelper(nop);
// 03 
make_EHelper(push);
// 04 
make_EHelper(sub);
// 05
make_EHelper(jmp);
// 06 
make_EHelper(xor);
// 07
make_EHelper(pop);
// 08
make_EHelper(ret); 

//9th 已经完成
make_EHelper(lea);
//10th 
make_EHelper(and);
//11th
make_EHelper(add);
//12th
make_EHelper(cmp);
//13th
make_EHelper(test);
//14
make_EHelper(setcc);
//15
make_EHelper(movzx);
//16th
make_EHelper(jcc);
//17th
make_EHelper(leave);
