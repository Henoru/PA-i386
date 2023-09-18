#include "cpu/instr.h"
#include "device/port_io.h"
/*
Put the implementations of `out' instructions here.
*/
make_instr_func(out_b){
  int len=1;
  decode_data_size_b
  decode_operand_a
  operand_read(&opr_src);
  //gpr[2]-> edx
  pio_write(cpu.gpr[2]._16,opr_src.data_size/8,opr_src.val);
  return len;
}
make_instr_func(out_v){
  int len=1;
  decode_data_size_v
  decode_operand_a
  operand_read(&opr_src);
  //gpr[2]-> edx
  pio_write(cpu.gpr[2]._16,opr_src.data_size/8,opr_src.val);
  return len;
}