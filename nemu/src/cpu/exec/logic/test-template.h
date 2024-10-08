#include "cpu/exec/template-start.h"

#define instr test

static void do_execute () {
	DATA_TYPE result = op_dest->val & op_src->val;

	update_eflags_pf_zf_sf((DATA_TYPE_S)result);
	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;

	Log("%x", cpu.eflags.ZF);

	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
