#include "cpu/exec/template-start.h"

#define instr setne

static void do_execute () {
	if(cpu.eflags.ZF == 0) {
        write_operand_b(op_src, 1);
	}
	else {
		write_operand_b(op_src, 0);
	}
	print_asm("setne %s", op_src->str);
}

make_instr_helper(rm)
#undef instr
#define instr sete

static void do_execute () {
	if(cpu.eflags.ZF == 1) {
        write_operand_b(op_src, 1);
	}
	else {
		write_operand_b(op_src, 0);
	}
	print_asm("sete %s", op_src->str);
}

make_instr_helper(rm)
#undef instr

#include "cpu/exec/template-end.h"
