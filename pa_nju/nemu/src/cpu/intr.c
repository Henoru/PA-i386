#include "cpu/intr.h"
#include "cpu/instr.h"
#include "memory/memory.h"

void raise_intr(uint8_t intr_no)
{
#ifdef IA32_INTR
	push(cpu.eflags.val);
	push(cpu.cs.val);
	push(cpu.eip);
	paddr_t paddr=cpu.idtr.base;
	#ifdef IA32_PAGE
		if(cpu.cr0.pg&&cpu.cr0.pe){
			paddr=page_translate(cpu.idtr.base);
		}
	#endif
	GateDesc* idt=(GateDesc*)(hw_mem+paddr)+intr_no;
	if(idt->type==0xe) cpu.eflags.IF=0;

	cpu.cs.val=idt->selector;
	cpu.eip=(idt->offset_31_16<<16)+idt->offset_15_0;
	//CS
	load_sreg(1);
#endif
}

void raise_sw_intr(uint8_t intr_no)
{
	// return address is the next instruction
	cpu.eip += 2;
	raise_intr(intr_no);
}
