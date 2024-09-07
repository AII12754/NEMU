#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
    cpu.esp -= 4;
    MEM_W(cpu.esp, cpu.eip + DATA_BYTE + 1);
    cpu.eip += op_src->val;
    print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
}

make_instr_helper(i)

/*
#if DATA_BYTE == 4

make_helper(call_rm_l) {
    cpu.esp -= DATA_BYTE;
    MEM_W(cpu.esp, cpu.eip);
    int len = decode_rm_l(cpu.eip + 1);
    cpu.eip = op_src->val;
    print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
}

#elif DATA_BYTE == 2

make_helper(call_rm_w) {
    cpu.esp -= DATA_BYTE;
    MEM_W(cpu.esp, cpu.eip);
    int len = decode_rm_w(cpu.eip + 1);
    cpu.eip = (op_src->val & 0x0000FFFF);
    print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
}

#endif
*/

#include "cpu/exec/template-end.h"
