#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
    // TODO: push
    cpu.esp -= DATA_BYTE;
    swaddr_write(cpu.esp, DATA_BYTE, cpu.eip);
    cpu.eip += op_src->val;
    print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
