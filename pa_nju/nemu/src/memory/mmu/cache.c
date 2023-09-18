#include "memory/mmu/cache.h"
#include "memory/memory.h"
#include <stdlib.h>
uint32_t hw_mem_read(paddr_t paddr, size_t len);
void hw_mem_write(paddr_t paddr, size_t len, uint32_t data);

struct {
    struct{
        struct{
            bool        valid;
            uint32_t    tag:19;
            union{
                uint8_t     data[64];
                uint16_t    data2[32];
                uint32_t    data4[16];
            };
        }block[8];
    }group[128];
}cache;
union mypaddr_t{
    struct{
        uint32_t data_id:6;     //  行内id
        uint32_t group_id:7;    //  组id
        uint32_t tag:19;
    };
    paddr_t paddr;
};

union mydata_t{
    uint8_t  __data__[4];
    uint32_t data;
};
// init the cache
void init_cache()
{
	for(uint32_t i=0;i<128;i++){
	    for(uint32_t j=0;j<8;j++){
	        cache.group[i].block[j].valid=false;
	    }
	}
}
#define BLOCK(x) cache.group[x.group_id].block
// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data)
{
    assert(paddr+len<=MEM_SIZE_B);
    // 全写法 + 非写分配
    hw_mem_write(paddr, len, data);
    
    union mypaddr_t mypaddr;
    union mydata_t mydata;
    
    mydata.data=data;

    for(size_t i=0;i<len;){
        mypaddr.paddr=paddr+i;
        bool success=false;
        for(uint32_t j=0;j<8;j++){
            if( BLOCK(mypaddr)[j].valid     &&
                BLOCK(mypaddr)[j].tag == mypaddr.tag){
                    success=true;
                    uint32_t k;
                    for(k=0;i+k<len && mypaddr.data_id+k<64; k++)
                        BLOCK(mypaddr)[j].data[mypaddr.data_id+k]=mydata.__data__[i+k];
                    i+=k;
                    break;
                }
        }
        if(!success) {
            if(len-i<64-mypaddr.data_id) i+=len-i;
            else i+=64-mypaddr.data_id;
        }
    }
    #ifdef CACHE_TEST
        uint32_t test=cache_read(paddr,len);
        assert(test==data);
    #endif
}

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len)
{
	// implement me in PA 3-1
	assert(paddr+len<=MEM_SIZE_B);
#ifdef CACHE_TEST
    uint32_t test=hw_mem_read(paddr,len);
	union mydata_t mytest;
    mytest.data=test;
#endif
	union mypaddr_t mypaddr;
    union mydata_t  mydata;
    mydata.data=0;
    for(size_t i=0;i<len;){
        mypaddr.paddr=paddr+i;
        bool success=false;
        uint32_t pos=8;
        // trying to match
        for(uint32_t j=0;j<8;j++){
            if( BLOCK(mypaddr)[j].valid     &&
                BLOCK(mypaddr)[j].tag == mypaddr.tag){
                // macthed
                    success=true;
                    uint32_t k;
                    for(k=0;i+k<len && mypaddr.data_id+k<64; k++)
                    {
                        mydata.__data__[i+k]=BLOCK(mypaddr)[j].data[mypaddr.data_id+k];
                        
                    #ifdef CACHE_TEST
                        assert(mydata.__data__[i+k]==mytest.__data__[i+k]);
                    #endif
                        
                    }
                    i+=k;
                    break;
                }
            if(!BLOCK(mypaddr)[j].valid) pos=j;
        }
        // umatch
        if(!success){
            if(pos>=8) pos=rand()%8;
            // Set the block
            BLOCK(mypaddr)[pos].valid=true;
            BLOCK(mypaddr)[pos].tag=mypaddr.tag;
            
            union mypaddr_t iter=mypaddr;
            // upd cache from mem
            for(uint32_t j=0;j<16;j++){
                iter.data_id=4*j;
                assert(iter.paddr+4<=MEM_SIZE_B);
                uint32_t tmp=hw_mem_read(iter.paddr,4);
                BLOCK(iter)[pos].data4[j]=tmp;
            }
            // fetch data frome cache
            uint32_t k;
            for(k=0;i+k<len && mypaddr.data_id+k<64; k++){
                mydata.__data__[i+k]=BLOCK(mypaddr)[pos].data[mypaddr.data_id+k];
                
                #ifdef CACHE_TEST
                    assert(mydata.__data__[i+k]==mytest.__data__[i+k]);
                #endif
                
            }
            i+=k;
        }
    }
    #ifdef CACHE_TEST
        assert(test==mydata.data);
	#endif
	return mydata.data;
}
#undef BLOCK
