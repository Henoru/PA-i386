#include "common.h"
#include "memory.h"
#include <string.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)
#define NR_PT ((SCR_SIZE + PT_SIZE - 1) / PT_SIZE) // number of page tables to cover the vmem
// #define NR_P (SCR_SIZE / PAGE_SIZE + 1) // number of pages to cover the vmem
PDE *get_updir();
PTE table[NR_PTE] align_to_page;
void create_video_mapping()
{

	/* TODO: create an identical mapping from virtual memory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memeory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */
	PDE *pdir = (PDE *)va_to_pa(get_updir());
	PTE *ptable;
	// assert(pdir->present);
	// assert(pdir->present==0);
	Log("create_video_mapping");
	if(pdir->present){
		ptable=(PTE*)(pdir->page_frame<<12);
	}else{
		ptable =(PTE *)va_to_pa(table);
		pdir->val=make_pde(ptable);
	}
	for(uint32_t i=VMEM_ADDR>>12;i<=(VMEM_ADDR+SCR_SIZE-1)>>12;i++){
			ptable[i].val=make_pte(i<<12);
	}
}

void video_mapping_write_test()
{
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for (i = 0; i < SCR_SIZE / 4; i++)
	{
		buf[i] = i;
	}
}

void video_mapping_read_test()
{
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for (i = 0; i < SCR_SIZE / 4; i++)
	{
		assert(buf[i] == i);
	}
}

void video_mapping_clear()
{
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}
