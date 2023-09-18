#include "cpu/instr.h"
void push(uint32_t val);
make_instr_func(call_rm_v){
    int len=1;
    decode_data_size_v
    decode_operand_rm
    operand_read(&opr_src);
    push(cpu.eip+len);
    print_asm_1("call","",len,&opr_src);
    cpu.eip=opr_src.val;
    return 0;
}
make_instr_func(call_i_v){
    int len=1;
    decode_data_size_v
    decode_operand_i
    operand_read(&opr_src);
    push(cpu.eip+len);
    print_asm_1("call","",len,&opr_src);
    cpu.eip+=opr_src.val;
    return len;
}
