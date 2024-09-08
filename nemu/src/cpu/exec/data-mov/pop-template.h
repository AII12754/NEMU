#include "cpu/exec/template-start.h"

#define instr pop

make_helper(concat(pop_, SUFFIX)) {
    REG(ops_decoded.opcode - 0x58) = MEM_R(cpu.esp);
	cpu.esp += DATA_BYTE;
	print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
	return 1;
}

#include "cpu/exec/template-end.h"