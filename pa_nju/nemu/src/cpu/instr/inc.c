#include "cpu/instr.h"
/*
Put the implementations of `inc' instructions here.
*/
static void instr_execute_1op(){
    operand_read(&opr_src);
    uint32_t tmp=cpu.eflags.CF;
    opr_src.val=alu_add(1,opr_src.val,opr_src.data_size);
    cpu.eflags.CF=tmp;
    operand_write(&opr_src);
}
make_instr_impl_1op(inc,r,v)
make_instr_impl_1op(inc,rm,v)