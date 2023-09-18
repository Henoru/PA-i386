#include "cpu/instr.h"
uint32_t pop() {
    opr_dest.type = OPR_MEM;
    opr_dest.sreg = SREG_SS;
    opr_dest.addr = cpu.esp;
    opr_dest.data_size = data_size;
    operand_read(&opr_dest);
    cpu.esp += opr_dest.data_size/ 8;
    return opr_dest.val;
}
static void instr_execute_1op(){
    opr_src.val=pop();
    operand_write(&opr_src);
}
make_instr_impl_1op(pop,r,v)
make_instr_impl_1op(pop,rm,v)
make_instr_func(popa){
    int len=1;
    cpu.edi=pop();
    cpu.esi=pop();
    cpu.ebp=pop();
    pop();
    cpu.ebx=pop();
    cpu.edx=pop();
    cpu.ecx=pop();
    cpu.eax=pop();
    return len;
}