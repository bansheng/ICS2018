#ifndef __REG_H__
#define __REG_H__

#include "common.h"
#include "memory/mmu.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */
#define REGSIZE 8
#define hlSIZE 2

typedef struct {
 

  /* Do NOT change the order of the GPRs' definitions. */
  
 
	union{ 
		struct{ 
		   rtlreg_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
		};
		union{ 
			paddr_t _32;
			ioaddr_t _16;
			bool _8[hlSIZE];
		} gpr[REGSIZE];
	};
	
	
	union {
		struct {
			uint8_t CF  :   1;
			uint8_t DEF1:	  1;
			uint8_t DEF2:	  4;
			uint8_t ZF  :   1;
			uint8_t SF  :   1;
			uint8_t DEF3:   1;
			uint8_t IF  :   1;
			uint8_t DEF4:   1;
			uint8_t OF  :   1;
			uint32_t DEF5:  20;
		} eflags;
		uint32_t flags;
	};
	
	bool INTR;

	struct {
		uint16_t limit;
		uint32_t base;
	} idtr;
	uint16_t cs;
	
	CR0 cr0;
  	CR3 cr3;
	
	vaddr_t eip;
} CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 8);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32) ///32位寄存器 
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16) //16位寄存器
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])// 8位寄存器

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

static inline const char* reg_name(int index, int width) {
  assert(index >= 0 && index < 8);
  switch (width) { 
    case 4: return regsl[index];
    case 1: return regsb[index];
    case 2: return regsw[index];
    default: assert(0);
  }
}

#endif
