#include "cpu/instr.h"
/*
Put the implementations of `lgdt' instructions here.
*/
make_instr_func(lgdt) {
    int len=1;
    decode_operand_rm
    
    // get limit
    opr_src.data_size=16;
    operand_read(&opr_src);
    cpu.gdtr.limit=opr_src.val;
    
    // get base
    opr_src.addr+=2;
    opr_src.data_size=32;
    operand_read(&opr_src);
    cpu.gdtr.base=opr_src.val;

    return len;
}