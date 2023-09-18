#include "cpu/cpu.h"

#define myMask(H) (0xFFFFFFFF>>(32-(H)))
#define getBit(x,H)  ((x)&(myMask((H))))
#define mySign(x,data_size) (sign(sign_ext((x),(data_size))))
void setPF(uint32_t res)
{
    res=getBit(res,8);
    uint32_t tmp=1;
    for(;res;res>>=1)
        tmp^=res&1;
    cpu.eflags.PF=tmp;
}
void setSF(uint32_t res,size_t data_size)
{
    cpu.eflags.SF=mySign(res,data_size);
}
void setOF_Add(uint32_t src, uint32_t dest, uint32_t res, size_t data_size){
    cpu.eflags.OF=(mySign(dest,data_size)==mySign(src,data_size))&&(mySign(dest,data_size)!=mySign(res,data_size));
}
void setOF_Sub(uint32_t src, uint32_t dest, uint32_t res, size_t data_size){
    cpu.eflags.OF=(mySign(dest,data_size)!=mySign(src,data_size))&&(mySign(dest,data_size)!=mySign(res,data_size));
}
uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
    dest=getBit(dest,data_size),src=getBit(src,data_size);
	uint32_t res=dest+src;
	res=getBit(res,data_size);
	// setCF
	cpu.eflags.CF=(res<dest);
	// setOF
	setOF_Add(src,dest,res,data_size);
	// setZF
	cpu.eflags.ZF=(res==0);
	// setSF
	setSF(res,data_size);
	// setPF
	setPF(res);
    return getBit(res,data_size);
#endif
}

uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
    dest=getBit(dest,data_size),src=getBit(src,data_size);
	uint32_t res=getBit(dest+src+cpu.eflags.CF,data_size);
	// setCF
	cpu.eflags.CF=(cpu.eflags.CF)?(res<=dest):(res<dest);
	// setOF
	setOF_Add(src,dest,res,data_size);
	// setZF
	cpu.eflags.ZF=(res==0);
	// setSF
	setSF(res,data_size);
	// setPF
	setPF(res);
	return res;
#endif
}

uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
    dest=getBit(dest,data_size),src=getBit(src,data_size);
	uint32_t res=getBit(dest-src,data_size);
	// setCF
	cpu.eflags.CF=(res>dest);
	// setOF
	setOF_Sub(src,dest,res,data_size);
	// setZF
	cpu.eflags.ZF=(res==0);
	// setSF
	setSF(res,data_size);
	// setPF
    setPF(res);
    return res;
#endif
}

uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
	dest=getBit(dest,data_size),src=getBit(src,data_size);
	uint32_t res=getBit(dest-(src+cpu.eflags.CF),data_size);
	// setCF
	cpu.eflags.CF=(cpu.eflags.CF)?(res>=dest):(res>dest);
	// setOF	
	setOF_Sub(src,dest,res,data_size);
	// setZF
	cpu.eflags.ZF=(res==0);
	// setSF
	setSF(res,data_size);
	// setPF
    setPF(res);
	return res;
#endif
}

uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
    dest=getBit(dest,data_size),src=getBit(src,data_size);
	uint64_t res=(uint64_t)src*(uint64_t)dest;
	if(data_size!=32) res=getBit(res,2*data_size);
	//setCF setOF
	cpu.eflags.OF=cpu.eflags.CF=((res>>data_size)!=0);
	return res;
#endif
}

int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
    dest=sign_ext(getBit(dest,data_size),data_size),src=sign_ext(getBit(src,data_size),data_size);
    int64_t res=(int64_t)dest*(int64_t)src;
    //setCF setOF
    cpu.eflags.OF=cpu.eflags.CF=(res>>data_size)!=0 && (res>>data_size)!=-1;
    // if(data_size!=32) res=getBit(res,2*data_size);
    return res;
#endif
}

// need to implement alu_mod before testing
uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
    if(src==0x0)
    {
        puts("Divided by 0!!!:alu_div");
        assert(0);
    }
    if(data_size!=32) dest=getBit(dest,2*data_size),src=getBit(src,2*data_size);
    uint32_t res=getBit(dest/src,data_size);
    return res;
#endif
}

// need to implement alu_imod before testing
int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
	if(src==0x0)
    {
        puts("Divided by 0!!!:alu_idiv");
        assert(0);
    }
    if(data_size!=32) dest=sign_ext_64(getBit(dest,2*data_size),2*data_size),src=sign_ext_64(getBit(src,2*data_size),2*data_size);
    int32_t res=getBit(dest/src,data_size);
	return res;
#endif
}
uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
	if(src==0x0)
    {
        puts("Divided by 0!!!:alu_mod");
        assert(0);
    }
    uint32_t res=dest%src;
    return res;
#endif
}

int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
	if(src==0x0)
    {
        puts("Divided by 0!!!:alu_mod");
        assert(0);
    }
    int32_t res=dest%src;
	return res;
#endif
}

uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
    dest=getBit(dest,data_size),src=getBit(src,data_size);
	uint32_t res=getBit(dest&src,data_size);
	// setCF setOF
	cpu.eflags.CF=cpu.eflags.OF=0;
	// setSF
	setSF(res,data_size);
	// setZF
	cpu.eflags.ZF=(res==0);
	// setPF
	setPF(res);
	return res;
#endif
}

uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
    dest=getBit(dest,data_size),src=getBit(src,data_size);
	uint32_t res=getBit(dest^src,data_size);
	// setCF setOF
	cpu.eflags.CF=cpu.eflags.OF=0;
	// setSF
	setSF(res,data_size);
	// setZF
	cpu.eflags.ZF=(res==0);
	// setPF
	setPF(res);
	return res;
#endif
}

uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
	dest=getBit(dest,data_size),src=getBit(src,data_size);
	uint32_t res=getBit(dest|src,data_size);
	// setCF setOF
	cpu.eflags.CF=cpu.eflags.OF=0;
	// setSF
	setSF(res,data_size);
	// setZF
	cpu.eflags.ZF=(res==0);
	// setPF
	setPF(res);
	return res;
#endif
}

uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
	dest=getBit(dest,data_size),src=getBit(src,data_size);
	uint32_t res=getBit(dest<<src,data_size);
	// setCF
	if(src!=0)
	    cpu.eflags.CF=(dest<<(src-1))>>(data_size-1);
	// setOF
	if(src==0x1)
	    cpu.eflags.OF=(res>>(data_size-1))^(cpu.eflags.CF);
	// setSF
	setSF(res,data_size);
	// setZF
	cpu.eflags.ZF=(res==0);
	// setPF
	setPF(res);
	return res;
#endif
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shr(src, dest, data_size);
#else
	dest=getBit(dest,data_size),src=getBit(src,data_size);
	uint32_t res=getBit(dest>>src,data_size);
	// setCF
	if(src!=0)
	    cpu.eflags.CF=(dest>>(src-1))&1;
	// setOF
	if(src==0x1)
	    cpu.eflags.OF=(src>>(data_size-1));
	// setSF
	setSF(res,data_size);
	// setZF
	cpu.eflags.ZF=(res==0);
	// setPF
	setPF(res);
	return res;
#endif
}

uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sar(src, dest, data_size);
#else
	dest=getBit(dest,data_size),src=getBit(src,data_size);
	int32_t tmp=sign_ext(dest,data_size);
	uint32_t res=getBit(tmp>>src,data_size);
	// setCF
	if(src!=0)
	    cpu.eflags.CF=(tmp>>(src-1))&0x1;
	// setOF
	if(src==0x1)
	    cpu.eflags.OF=0;
	// setSF
	setSF(res,data_size);
	// setZF
	cpu.eflags.ZF=(res==0);
	// setPF
	setPF(res);
	return res;
#endif
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
    return alu_shl(src,dest,data_size);
#endif
}
