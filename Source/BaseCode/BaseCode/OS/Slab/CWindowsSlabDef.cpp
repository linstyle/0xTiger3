
#include "CWindowsSlabDef.h"
#include "CVirPageHead.h"
#include "GlobalMacro.h"
using namespace name_slab;

long name_slab::CalcSingleObjectHeadSize()
{
	long nUseObjectSize=0;
	if( DEBUG_SLAB_FALGS & SLAB_RED_ZONE )
	{
		nUseObjectSize += sizeof(HEAD_BAD_ALIEN_MAGIC);
	}

	nUseObjectSize += HEAD_PAGE_ADDR_SIZE;

	return nUseObjectSize;
}

long name_slab::CalcSingleObjectSize(long nUseObjectSize)
{
	nUseObjectSize += CalcSingleObjectHeadSize();

	nUseObjectSize = _ALIGN(nUseObjectSize, __ALIGN_SIZE_LONG);

	return nUseObjectSize;		
}

//访问到页面头地址
VIR_PAGE_HEAD* name_slab::OffsetPageHead(void *p)
{
	//小端机
	long *_p = (long*) ( (long)p-HEAD_PAGE_ADDR_SIZE );
	VIR_PAGE_HEAD *pPage =(VIR_PAGE_HEAD* ) (*_p);
	if (HEAD_BAD_ALIEN_MAGIC!=pPage->m_nBadAlienMagic)
	{
		return NULL;
	}

	return pPage;
}

//访问到页面魔术数字
unsigned int name_slab::OffsetBadMagic(void *p)
{
	//小端机
	return *(unsigned int*)( (long)p-HEAD_PAGE_ADDR_SIZE-sizeof(HEAD_BAD_ALIEN_MAGIC) );
}

//访问当前CPUID
int name_slab::GetCPURunId()
{
	return 0;
	//_asm {mov eax, 1} 
	//_asm {cpuid} 
	//_asm {shr ebx, 24} 
	//_asm {mov eax, ebx} 
}