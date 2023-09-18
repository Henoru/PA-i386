#include "cpu/instr.h"
#include "device/port_io.h"
/*
Put the implementations of `in' instructions here.
*/
make_instr_func(in_b){
  int len=1;
  decode_data_size_b
  decode_operand_a
  //gpr[2]-> edx
  opr_src.val=pio_read(cpu.gpr[2]._16,opr_src.data_size/8);
  operand_write(&opr_src);
  return len;
}
make_instr_func(in_v){
  int len=1;
  decode_data_size_v
  decode_operand_a
  //gpr[2]-> edx
  opr_src.val=pio_read(cpu.gpr[2]._16,opr_src.data_size/8);
  operand_write(&opr_src);
  return len;
}