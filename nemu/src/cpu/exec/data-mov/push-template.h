#include "cpu/exec/template-start.h"

#define instr push

/*
static void do_execute() {
	cpu.esp -= DATA_BYTE;
    cpu.esp = REG(ops_decoded.opcode - 0x50);
	print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(i)
*/

make_helper(concat(push_, SUFFIX)) {
	cpu.esp -= DATA_BYTE;
    cpu.esp = REG(ops_decoded.opcode - 0x50);
	print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
	return 1;
}

#include "cpu/exec/template-end.h"