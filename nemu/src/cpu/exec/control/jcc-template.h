#include "cpu/exec/template-start.h"

#define instr jcc

#if DATA_BYTE == 1
make_helper(jcc_z) {
    if(cpu.eflags.ZF == 1) {
        cpu.eip += op_src->val;
    }
    return 2;
}
#endif

#include "cpu/exec/template-end.h"
