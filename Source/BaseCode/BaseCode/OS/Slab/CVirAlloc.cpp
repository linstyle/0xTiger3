#include "StdAfx.h"
#include "CVirAlloc.h"
#include <windows.h>

/*
	CVirPageAddress
*/
CVirPageAddress::CVirPageAddress()
{
	m_AllocAddress.reserve(name_viralloc::ALLOC_COUNT);
}
void CVirPageAddress::Add(unsigned long Start, unsigned long End)
{
	assert( Start>0 && End>0);
	ALLOC_ADDRESS AllocAddress(Start, End);

	m_AllocAddress.push_back(AllocAddress);
}

int CVirPageAddress::GetAddressIndex(unsigned long nAddress)
{
	ALLOC_ADDRESS AllocAddress;
	for(int i=0; i<name_viralloc::ALLOC_COUNT; ++i)
	{	
		AllocAddress = m_AllocAddress[i];
		if(nAddress>=AllocAddress.m_Start && nAddress<AllocAddress.m_End)
		{
			return i;
		}
	}

	return -1;
}

/*
	CVirPageTable
*/
void CVirPageTable::Init(unsigned long Start, unsigned long Offset, unsigned long End)
{
	assert( Start>0 && End>0 && Offset>0);
	m_Start = Start;
	m_Offset = Offset;
	m_End = End;
}

long CVirPageTable::ToPageOffset(void* pUnknowStart)
{
	unsigned long nUnknowStart = (unsigned long)pUnknowStart;

	/*
	小端机，按顺序地址
	0x1, 0x2, 0x3
	0x4, 0x5, 0x6
	0x7, 0x8, 0x9
	*/
	if( (nUnknowStart>=m_Start) && (nUnknowStart<m_End) )
	{
		return (nUnknowStart-m_Start) / m_Offset;
	}

	return -1;
}

/*
	CVirPageHead
*/
CVirPageHead::~CVirPageHead()
{
	delete []m_pVirPageHead;
}

void CVirPageHead::Init(long nPageCount)
{
	m_nPageCount = nPageCount;
	m_pVirPageHead = new VIR_PAGE_HEAD[m_nPageCount];	
}

VIR_PAGE_HEAD* CVirPageHead::GetPageHead(long nPageIndex)
{
	if(nPageIndex>=m_nPageCount)
	{
		return NULL;
	}

	return &m_pVirPageHead[nPageIndex];
}

/*
	CVirPageAssis
*/
void CVirPageAssis::InitPageHead(long nPageCount)
{
	m_VirPageHead.Init(nPageCount);
}
void CVirPageAssis::InitPageTable(unsigned long Start, unsigned long Offset, unsigned long End)
{	
	m_VirPageTable.Init(Start, Offset, End);
}

VIR_PAGE_HEAD* CVirPageAssis::GetPageHead(void *p)
{
	long nIndex = GetPageHeadIndex(p);

	return m_VirPageHead.GetPageHead(nIndex);
}

long CVirPageAssis::GetPageHeadIndex(void* p)
{
	return m_VirPageTable.ToPageOffset(p);
}


/*
	VirtualAlloc函数以byte 为单位
*/
CVirAlloc::CVirAlloc()
{
	m_pDebugFlag = NULL;
	m_nUsePageCount = 0;
}

CVirAlloc::~CVirAlloc()
{
	VirtualFree(m_pAddressBegin, 0, MEM_RELEASE);
	delete []m_pDebugFlag;
}

void CVirAlloc::Init(long nAllocMB, long nAllocGrainKB)
{
	m_nAllocMemMB = nAllocMB;
	m_nAllocMemKB = m_nAllocMemMB*1024;
	m_nAllocMemByte = m_nAllocMemKB*1024;

	m_nAllocGrainKB = nAllocGrainKB;
	m_nAllocGrainByte = m_nAllocGrainKB*1024;

	InitAllocMem();
	InitVirPageAssis();
	InitPageOffset();
}

void CVirAlloc::InitAllocMem()
{
	m_pAddressBegin = VirtualAlloc(NULL, m_nAllocMemByte, MEM_RESERVE, PAGE_READWRITE);
	INITASSERT( NULL==m_pAddressBegin);

	m_pAddressEnd = (void*)((unsigned long)m_pAddressBegin+m_nAllocMemByte);
}

void CVirAlloc::InitVirPageAssis()
{
	m_VirPageAssis.InitPageHead(GetPageCount());
	m_VirPageAssis.InitPageTable((unsigned long)m_pAddressBegin, m_nAllocGrainByte, (unsigned long)m_pAddressEnd);
}

void CVirAlloc::InitPageOffset()
{	
	/*
		预设一个总的页面数，然后压入各个页面的下标
	*/
	long nCount = GetPageCount();
	
	m_PageOffset.reserve(nCount);
	for(long i=0; i<nCount; ++i)
	{
		m_PageOffset.push_back(i);
	}

	m_pDebugFlag = new long[nCount/name_viralloc::BIT_MAP_SIZE+1];  //防止除时有余数
	memset(m_pDebugFlag, 0, sizeof(long)*(nCount/name_viralloc::BIT_MAP_SIZE+1));
}

VIR_PAGE_HEAD* CVirAlloc::MallocPage()
{
	/*
		取出一个偏移值
		并且检测是否被重复获取
	*/
	if(m_PageOffset.size() <=0 )
	{
		return NULL;
	}

	long nOffset;

	m_FastLock.Lock();
	nOffset = __MallocPage();
	m_FastLock.UnLock();

	if( -1==nOffset )
	{
		printf("Err, CVirAlloc::FreePage:__MallocPage()\n");
		return NULL;
	}

	void *p;
	VIR_PAGE_HEAD* pPage;

	p = VirtualAlloc( (LPVOID)((unsigned long)m_pAddressBegin + nOffset*m_nAllocGrainByte), 
		             m_nAllocGrainByte, MEM_COMMIT, PAGE_READWRITE);
	
	pPage = m_VirPageAssis.GetPageHead(p);
	if(NULL==pPage)
	{
		printf("Err, VIR_PAGE_HEAD* CVirAlloc::MallocPage():NULL==pPage\n");
		return NULL;
	}
	pPage->m_pFreeList = p;

	/*
		计数器
	*/
	m_nUsePageCount++;

	return pPage;
}

void  CVirAlloc::FreePage(VIR_PAGE_HEAD *pPage)
{
	/*
		从表里查到该页在总调拨器里的偏移位置，然后释放。
		并且检测是否被重复释放
		最后把该偏移量放入数组，以供下回取出
	*/
	int nResult;
	long nOffset;

	nOffset = m_VirPageAssis.GetPageHeadIndex(pPage->m_pFreeList);
	if( -1==nOffset )
	{
		printf("Err, CVirAlloc::FreePage:m_VirPageTable.ToPageOffset\n");
		return;
	}

	m_FastLock.Lock();
	nResult = __FreePage(nOffset);
	m_FastLock.UnLock();
	if( -1==nResult )
	{
		printf("Err, CVirAlloc::FreePage:__FreePage().\n");
		return;
	}

	/*
		在释放时就做初始化动作，可以防止被重复使用而报错
	*/
	pPage->Init();

	VirtualFree( (LPVOID)((unsigned long)m_pAddressBegin + nOffset*m_nAllocGrainByte), 
				 m_nAllocGrainByte, MEM_DECOMMIT);
}

VIR_PAGE_HEAD* CVirAlloc::GetPageHead(void *p)
{
	return m_VirPageAssis.GetPageHead(p);
}


long CVirAlloc::__MallocPage()
{
	long nOffset;

	nOffset = m_PageOffset.back();
	if( 1==GetDebugFlag(nOffset) )
	{
		printf("Err, CVirAlloc::__MallocPage(), Has Malloc.Offset:%d\n", nOffset);
		return -1;
	}

	m_PageOffset.pop_back();
	SetDebugFlag(nOffset, 1);

	return nOffset;
}

long CVirAlloc::__FreePage(long nOffset)
{
	if( 0==GetDebugFlag(nOffset) )
	{
		printf("Err, CVirAlloc:::__FreePage, Has Free.Offset:%d\n", nOffset);
		return -1;
	}

	SetDebugFlag(nOffset, 0);
	m_PageOffset.push_back(nOffset);

	return 0;
}

void CVirAlloc::SetDebugFlag(long nOffset, long nFlag)
{
	long nMultiple = nOffset/name_viralloc::BIT_MAP_SIZE;
	long *p = &m_pDebugFlag[nMultiple];

	nOffset -= nMultiple*name_viralloc::BIT_MAP_SIZE;

	/*
		先将目标位清零，再进行赋值。这样就避免了，如果原来值是1，赋值是0，| 作用符不起作用
	*/
	*p &= ~(1<<nOffset);
	*p |= (nFlag<<nOffset);
}

long CVirAlloc::GetDebugFlag(long nOffset)
{
	long nMultiple = nOffset/name_viralloc::BIT_MAP_SIZE;
	long *p = &m_pDebugFlag[nMultiple];

	nOffset -= nMultiple*name_viralloc::BIT_MAP_SIZE;

	return (*p>>nOffset) & 0x1;
}

long CVirAlloc::GetPageCount()
{
	return m_nAllocMemKB/m_nAllocGrainKB;
}

long CVirAlloc::GetFreePageCount()
{
	long n=0;
	long nPageCount = GetPageCount();
	
	for(int i=0; i<nPageCount; ++i)
	{
		if( 0==GetDebugFlag(i) )
		{
			++n;
		}
	}

	return n;
}

long CVirAlloc::GetUsePageCount()
{
	long n=0;
	long nPageCount = GetPageCount();

	for(int i=0; i<nPageCount; ++i)
	{
		if( 1==GetDebugFlag(i) )
		{
			++n;
		}
	}

	return n;
}

long CVirAlloc::GetAllocMemMB()
{
	return m_nAllocMemMB;
}

long CVirAlloc::GetAllocGrainByte()
{
	return m_nAllocGrainByte;
}

void* CVirAlloc::GetAddressBegin()
{
	return m_pAddressBegin;
}
void* CVirAlloc::GetAddressEnd()
{
	return m_pAddressEnd;
}

/*
	CVirAllocManager
*/
CVirAllocManager::CVirAllocManager()
{
	Init();
}

void CVirAllocManager::Init()
{
	for(int i=0; i<name_viralloc::ALLOC_COUNT; ++i)
	{
		long nAllocGrainKB = name_viralloc::ALLOC_GRAIN_SIZE_KB[i];
		long nAllocMB = name_viralloc::ALLOC_MEM_SIZE_MB[i];
		
		m_VirAlloc[i].Init(nAllocMB, nAllocGrainKB);
		m_VirPageAddress.Add((unsigned long)m_VirAlloc[i].GetAddressBegin(), (unsigned long)m_VirAlloc[i].GetAddressEnd());
	}
}

CVirAlloc* CVirAllocManager::GetVirAlloc(long nSizeByte)
{
	long nIndex = GetVirAllocIndex(nSizeByte);
	if(-1==nIndex)
	{
		return NULL;
	}

	return &m_VirAlloc[nIndex];
}

long CVirAllocManager::GetVirAllocIndex(long nSizeByte)
{
		
	for(int i=0; i<name_viralloc::ALLOC_COUNT; ++i)
	{
		long nAllocGrainByte = m_VirAlloc[i].GetAllocGrainByte();
		if( nSizeByte<= nAllocGrainByte)
		{
			return i;
		}
	}

	return -1;
}

VIR_PAGE_HEAD* CVirAllocManager::GetPageHead(void *p)
{
	int nAddressIndex = m_VirPageAddress.GetAddressIndex((unsigned long)p);

	if(-1==nAddressIndex)
	{
		return NULL;
	}

	return m_VirAlloc[nAddressIndex].GetPageHead(p);
}

void CVirAllocManager::TracerMe()
{
	DELAY_CONTINUE(1000*1)
	Tracer();
}

void CVirAllocManager::Tracer()
{
	for(int i=0; i<name_viralloc::ALLOC_COUNT; ++i)
	{
		CVirAlloc *pVirAlloc = &m_VirAlloc[i];

		printf("Notic: CVirAllocManager\n");
		printf("	NO.%d:\n", i);
		printf("AllocMemMB:%dMB, AllocGrainKB:%dKB\n", 
			    pVirAlloc->GetAllocMemMB(), pVirAlloc->GetAllocGrainByte()/1024);
		printf("(Page)Sum Count:%d, Has Use Count:%d, Has Free Count:%d\n", 
			    pVirAlloc->GetPageCount(), pVirAlloc->GetUsePageCount(), pVirAlloc->GetFreePageCount());

		printf("\n");
	}
}