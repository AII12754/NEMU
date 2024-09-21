#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute(){
    int val;
    if(DATA_BYTE == 2){
        val = swaddr_read(cpu.esp, DATA_BYTE);
        cpu.esp += DATA_BYTE;
    }
    else{
        val = swaddr_read(cpu.esp, 4);
        cpu.esp += 4;
    }
    OPERAND_W(op_src, val);
    print_asm_template1();
}
make_instr_helper(r);

#include "cpu/exec/template-end.h"
