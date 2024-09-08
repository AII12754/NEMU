#include "cpu/exec/template-start.h"

#define instr jcc_z

static void do_execute() {
    if(cpu.eflags.ZF == 1) {
        cpu.eip += op_src->val;
    }
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
