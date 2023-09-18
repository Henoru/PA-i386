#include "cpu/instr.h"
/*
Put the implementations of `ret' instructions here.
*/
uint32_t pop();
make_instr_func(ret_near){
    print_asm_0("ret","",1);
    cpu.eip=pop();
    return 0;
}
make_instr_func(ret_near_imm16){
    int len=1;
    decode_data_size_w
    decode_operand_i
    operand_read(&opr_src);
    print_asm_1("ret","",len,&opr_src);
    cpu.eip=pop();
    cpu.esp+= opr_src.val;
    return 0;
}