#include "cpu/cpu.h"
#include "memory/memory.h"

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED
	pageaddr myaddr;
	myaddr.addr=laddr;
	PDE* pde=(PDE*)(hw_mem+(cpu.cr3.pdbr<<12)+(myaddr.dir<<2));
	assert(pde->present==1);
	PTE* pte=(PTE*)(hw_mem+((pde->page_frame)<<12)+(myaddr.page<<2));
	assert(pte->present==1);
	paddr_t paddr=(((pte->page_frame)<<12)+myaddr.offset);
	return paddr;
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
