#include "cpu/exec/template-start.h"

//#define instr ret

make_helper(concat(ret_, SUFFIX)) {
    cpu.eip = MEM_R(cpu.esp) - 1;
    cpu.esp += 4;
    return 1;
}

#include "cpu/exec/template-end.h"
