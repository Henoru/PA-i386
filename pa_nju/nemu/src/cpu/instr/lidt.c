#include "cpu/instr.h"
/*
Put the implementations of `lidt' instructions here.
*/
make_instr_func(lidt){
  int len=1;
  decode_operand_rm
  // get limit
  opr_src.data_size=16;
  operand_read(&opr_src);
  cpu.idtr.limit=opr_src.val;
  
  // get base
  opr_src.addr+=2;
  opr_src.data_size=32;
  operand_read(&opr_src);
  cpu.idtr.base=opr_src.val;

  return len;
}