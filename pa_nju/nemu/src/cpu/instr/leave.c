#include "cpu/instr.h"
/*
Put the implementations of `leave' instructions here.
*/
uint32_t pop();
make_instr_func(leave){
    int len=1;
    cpu.esp=cpu.ebp;
    cpu.ebp=pop();
    print_asm_0("leave","",1);
    return len;
}