#include "cpu/instr.h"
#define make_my_jmp(src_type,suffix) \
	make_instr_func(concat5(jmp, _, src_type, _, suffix))                                                                 \
	{                                                                                                                           \
		int len = 1;                                                                                                            \
		concat(decode_data_size_, suffix)                                                                                       \
		concat3(decode_operand, _, src_type)                                                                                \
		print_asm_1("jmp", opr_src.data_size == 8 ? "b" : (opr_src.data_size == 16 ? "w" : "l"), len, &opr_src); \
		operand_read(&opr_src); \
        if(opr_src.type==OPR_IMM){\
            cpu.eip += sign_ext(opr_src.val, opr_src.data_size);\
        }\
        else{\
            cpu.eip=sign_ext(opr_src.val, opr_src.data_size);\
            len=0;\
        }                                                                                                 \
		return len;                                                                                                             \
	}
make_my_jmp(i, near)
make_my_jmp(rm, near)
make_my_jmp(rm, short_)
make_my_jmp(i,short_)
// make_instr_func(jmp_short){
//     int len=1;
//     decode_data_size_short_
//     decode_operand_i
//     operand_read(&opr_src);
//     eip+=sign_ext(opr_src.val,opr_src.data_size);
//     return len;
// }
// static void instr_execute_1op() 
// {
//     operand_read(&opr_src);
//     cpu.eip += sign_ext(opr_src.val, opr_src.data_size);
// }
// make_instr_impl_1op(jmp, i, short_)
make_instr_func(jmp_far_imm)
{
    int len=1;
    decode_operand_i
    operand_read(&opr_src);
    cpu.eip=opr_src.val;
    
    opr_src.addr+=data_size/8;
    opr_src.data_size=16;
    operand_read(&opr_src);
    cpu.cs.val=opr_src.val;
    
    return 0;
}
