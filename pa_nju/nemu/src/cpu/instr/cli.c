#include "cpu/instr.h"
/*
Put the implementations of `cli' instructions here.
*/
make_instr_func(cli){
  int len=1;
  cpu.eflags.IF=0;
  return len;
}