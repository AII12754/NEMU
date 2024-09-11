#include "cpu/exec/template-start.h"

#define instr je
static void do_execute() {
    if(cpu.eflags.ZF == 1) {
        cpu.eip = op_src->val;
    }
}
make_instr_helper(i)
#undef instr

#define instr jbe
static void do_execute() {
    if(cpu.eflags.ZF == 1 || cpu.eflags.CF == 1) {
        cpu.eip = op_src->val;
    }
}
make_instr_helper(i)
#undef instr

#define instr jne
static void do_execute() {
    if(cpu.eflags.ZF == 0) {
        cpu.eip = op_src->val;
    }
}
make_instr_helper(i)
#undef instr

#include "cpu/exec/template-end.h"
