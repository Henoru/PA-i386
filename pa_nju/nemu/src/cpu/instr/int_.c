#include "cpu/intr.h"
#include "cpu/instr.h"

/*
Put the implementations of `int' instructions here.

Special note for `int': please use the instruction name `int_' instead of `int'.
*/
make_instr_func(int_){
  int len=0;
  decode_data_size_b
  decode_operand_i

  operand_read(&opr_src);
  raise_sw_intr(opr_src.val);

  return 0;
}