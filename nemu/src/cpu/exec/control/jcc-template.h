#include "cpu/exec/template-start.h"

#if instr == je
static void do_execute() {
    if(cpu.eflags.ZF == 1) {
        cpu.eip += op_src->val;
    }
}
#elif instr == jbe
static void do_execute() {
    if(cpu.eflags.ZF == 1 || cpu.eflags.CF == 1) {
        cpu.eip += op_src->val;
    }
}
#endif

make_instr_helper(i)

#include "cpu/exec/template-end.h"
