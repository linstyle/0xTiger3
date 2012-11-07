#include "StdAfx.h"
#include "CMyStackWalker.h"
#include <algorithm>

namespace 
{
	CKmemManager _KmemManager;
};

CBadAlienMagic g_BadAlienMagic;

CSlabCache::CSlabCache()
{
	m_nOffset = 0;
	m_pFreeList = NULL;	
	m_pPage = NULL;
}

/*
	CSlabNode
*/
CSlabNode::CSlabNode()
{
	INIT_LIST_HEAD(&m_PartialList);
	m_nPartialListCount = 0;
}

VIR_PAGE_HEAD* CSlabNode::PopPageHead()
{
	VIR_PAGE_HEAD* p;

	p = __PopPageHead();

	return p;
}

void CSlabNode::PopPageHead(VIR_PAGE_HEAD* p)
{
	list_del(&p->m_PageNext);	
	--m_nPartialListCount;
}

void CSlabNode::PushPageHead(VIR_PAGE_HEAD* p)
{		
	__PushPageHead(p);
}

unsigned long CSlabNode::GetPartialCount()
{
	return m_nPartialListCount;
}

VIR_PAGE_HEAD* CSlabNode::__PopPageHead()
{
	list_head *pListIte;
	VIR_PAGE_HEAD *pPackHead;

	pListIte = list_get(&m_PartialList);
	if(NULL==pListIte)
	{
		return NULL;
	}

	pPackHead = list_entry_offset(pListIte, VIR_PAGE_HEAD, m_PageNext);


	--m_nPartialListCount;
	return pPackHead;
}

void CSlabNode::__PushPageHead(VIR_PAGE_HEAD* p)
{	
	++m_nPartialListCount;
	list_add(&p->m_PageNext, &m_PartialList);
}

/*
	CKmem
*/
void CKmem::Init(const char* strKmemName, int nObjectSizeByte, int nObjectMetaSizeByte, CVirAlloc* pVirAlloc)
{
	n_mCPUNumberOf = GetCPUNumberOf();
	m_pSlabCache = new CSlabCache[n_mCPUNumberOf];

	m_nBadAlienMagic = g_BadAlienMagic.GetBadAlienMagic();
	m_nHasFreePage = m_nCountMallocPartial = m_nCountFree = m_nCountMalloc = m_nHasUsePage = 0;

	/*
		需要参数的初始化
	*/
	memset(m_szName, 0, name_slab::KMEM_NAME_SIZE);
	strncpy(m_szName, strKmemName, name_slab::KMEM_NAME_SIZE-4);

	m_nObjectSizeByte = nObjectSizeByte;
	m_nObjectMetaSizeByte = nObjectMetaSizeByte;
	m_pWinVirAlloc=pVirAlloc;

	/*
		初始化保留页面
	*/
	int nMinPartial;
	nMinPartial = m_pWinVirAlloc->GetPageCount()*(name_slab::MIN_PARTIAL_PER/100.0);
//	nMinPartial = 400;
	SetMinPartial(nMinPartial);
}

CKmem::~CKmem()
{
	delete []m_pSlabCache;
}

unsigned long CKmem::GetMinPartial()
{
	return m_nMinPartial;
}
void CKmem::SetMinPartial(int nMinPartial)
{
	m_nMinPartial = nMinPartial;
}

int CKmem::GetObjectSizeByte()
{
	return m_nObjectSizeByte;
}

int CKmem::GetObjectMetaSizeByte()
{
	return m_nObjectMetaSizeByte;
}

const char*CKmem::GetKmemName()
{
	return m_szName;
}

void* CKmem::GetBadAlienMagicAddr(void *p)
{
	return (void*)( (unsigned int)p + GetObjectMetaSizeByte()-sizeof(name_slab::BAD_ALIEN_MAGIC) );
}

CSlabNode* CKmem::GetSlabNode()
{
	return &m_SlabNode;
}

CVirAlloc* CKmem::GetVirAlloc()
{
	return m_pWinVirAlloc;
}

void* CKmem::KmemMalloc()
{	
	m_FastLock.Lock();

	CSlabCache *s = GetCpuSlab();
	if(NULL==s->m_pFreeList)
	{
		if(NULL==MallocPage(s))
		{
			m_FastLock.UnLock();
			return NULL;
		}
	}

	void *p = s->m_pFreeList;
	VIR_PAGE_HEAD* pPage = m_pWinVirAlloc->GetPageHead(p);
	IF(OS_LOCK==pPage->m_nState);
	{
		m_FastLock.UnLock();
		return NULL;
	}
	s->m_pFreeList = (void*)(*(unsigned int*)p);
	
	++pPage->m_nHasUseObject;
	++m_nCountMalloc;
	
	m_FastLock.UnLock();

	return p;
}

void CKmem::KmemFree(void *p)
{
	IF(NULL==p)
	{
		return;	
	}

	/*
		1.判断是否和当前页面相等，相等则释放
		2.不相等，则计算到pagehead,插入到邻居节点
	*/
	VIR_PAGE_HEAD* pPage = m_pWinVirAlloc->GetPageHead(p);
	IF(NULL==pPage)
	{
		return;
	}

	if( DEBUG_DEFAULT_FALGS & SLAB_RED_ZONE )
	{	
		IF( m_nBadAlienMagic!=*(unsigned int*)GetBadAlienMagicAddr(p) )
		{
			return;
		}
	}

	m_FastLock.Lock();

	if(false==FreeLocal(pPage,p))
	{
		FreeNode(pPage, p);
	}
	
	++m_nCountFree;
	m_FastLock.UnLock();
}

bool CKmem::FreeLocal(VIR_PAGE_HEAD* pPage,void *p)
{
	int nCPUCount=1;
	if( DEBUG_DEFAULT_FALGS & CPU_RUN_ID )
	{	
		nCPUCount = n_mCPUNumberOf;
	}

	for(int i=0; i<nCPUCount; ++i)
	{
		CSlabCache *SlabVerify = GetSlab(i);
		IF(NULL==SlabVerify)
		{
			return false;
		}

		if( pPage==m_pWinVirAlloc->GetPageHead(SlabVerify->m_pFreeList) )
		{
			*(unsigned int*)p = (unsigned int)SlabVerify->m_pFreeList;
			SlabVerify->m_pFreeList = p;
			--pPage->m_nHasUseObject;

			return true;
		}
	}

	return false;
}

void CKmem::FreeNode( VIR_PAGE_HEAD* pPage, void *p)
{	
	void *pPrior = pPage->m_pFreeList;

	*(unsigned int*)p = (unsigned int)pPage->m_pFreeList;
	pPage->m_pFreeList = p;
	--pPage->m_nHasUseObject;

	/*
		该页面的第一次释放
	*/
	if( NULL==pPrior )
	{
		pPage->m_nState = PARTIAL_LOCK;	 
		m_SlabNode.PushPageHead(pPage);
		return;
	}

	/*
		释放内存给系统,从邻居节点中删除
	*/
	if( !(DEBUG_DEFAULT_FALGS & GC_PARTIAL_PAGE) )
	{
		return;
	}

	if( pPage->m_nHasUseObject==0 && (m_SlabNode.GetPartialCount() >= GetMinPartial()) )
	{			
		m_SlabNode.PopPageHead(pPage);
		m_pWinVirAlloc->FreePage(pPage);

		++m_nHasFreePage;
		pPage->m_nState = OS_LOCK;
	}
}

void CKmem::InitMemoryList(void *_p)
{
	IF(NULL==_p)
	{
		return;
	}

	void *p = _p;

	int i = m_pWinVirAlloc->GetAllocGrainByte()/GetObjectMetaSizeByte();
	
	for(; i>0; --i)	
	{				
		if( DEBUG_DEFAULT_FALGS & SLAB_RED_ZONE )
		{
			*(unsigned int*)GetBadAlienMagicAddr(p) = m_nBadAlienMagic;
		}

		*(unsigned int*)p = (unsigned int)p + GetObjectMetaSizeByte();
		p = (void*)((unsigned int)p + GetObjectMetaSizeByte());
	}

	p = (void*)((unsigned int)p - GetObjectMetaSizeByte());
	*(unsigned int*)p = NULL;	
}

CSlabCache *CKmem::GetCpuSlab()
{
	int i=GetCPURunId();
	if( DEBUG_DEFAULT_FALGS & CPU_RUN_ID )
	{
		//printf("CKmem::GetCpuSlab:%d\n", i);
	}
	else
	{
		i = 0;
	}
	return &m_pSlabCache[i];
}

CSlabCache *CKmem::GetSlab(int nIndex)
{
	IF(nIndex>=GetCPUNumberOf())
	{
		return NULL;
	}
	return &m_pSlabCache[nIndex];
}

void* CKmem::MallocPage(CSlabCache *s)
{
	/*
		1.如果为空，则寻找邻居节点
					如果邻居节点为空，申请内存
		2.如果不为空，直接操作链表返回
	*/	
	if( NULL==MallocPartialNode(s) )
	{
		IF( NULL==MallocOSPage(s) )
		{
			return NULL;
		}
	}

	VIR_PAGE_HEAD* pPage = m_pWinVirAlloc->GetPageHead(s->m_pFreeList);
	pPage->m_nState = SLAB_LOCK;

	return s->m_pFreeList;
}

void* CKmem::MallocPartialNode(CSlabCache *s)
{
	VIR_PAGE_HEAD *pPage = m_SlabNode.PopPageHead();

	if(NULL==pPage)
	{
		return NULL;
	}
		
	s->m_pFreeList = pPage->m_pFreeList;
	pPage->m_pFreeList = NULL;	

	++m_nCountMallocPartial;
	return s->m_pFreeList;
}

void* CKmem::MallocOSPage(CSlabCache *s)
{	
	VIR_PAGE_HEAD *pPage = m_pWinVirAlloc->MallocPage();

	if(NULL==pPage)
	{
		return NULL;
	}

	s->m_pPage = pPage;
	s->m_pFreeList = pPage->m_pFreeList;

	/*
		初始化page一些信息
	*/
	pPage->m_pFreeList = NULL;
	pPage->m_nHasUseObject = 0;
	pPage->m_pKmem = this;	
	
	InitMemoryList(s->m_pFreeList);

	++m_nHasUsePage;
	return s->m_pFreeList;
}

initialiseSingleton(CKmemManager);
/*
	CKmemManager
*/
CKmemManager::CKmemManager()
{
	Init();
}
void CKmemManager::TracerMe()
{		
	DELAY_CONTINUE(1000*15)
	Tracer();
}

void CKmemManager::Tracer()
{
	printf("Notic: CKmemManager\n");
	for(unsigned int i=0; i<name_slab::KMEM_COUNT; ++i)
	{
		CKmem *pKmem = &m_Kmem[i];
		long nVirAllocGrainByte = pKmem->GetVirAlloc()->GetAllocGrainByte();

		printf("	NO.%d, Name:%s \n", i, pKmem->GetKmemName());
		printf("(Object)ObjectMetaSizeByte:%d, Single Page Objects:%d\n", pKmem->GetObjectMetaSizeByte(), nVirAllocGrainByte/pKmem->GetObjectMetaSizeByte());
		printf("(Mount)Viralloc AllocGrainKB:%dKB\n", nVirAllocGrainByte/1024);
		printf("(Count)Has Malloc Count:%d, Has Free Count:%d, Partial Malloc Count:%d\n", 
				pKmem->m_nCountMalloc, pKmem->m_nCountFree, pKmem->m_nCountMallocPartial);
		printf("(Page)Has Use Count:%d, Has Free Count:%d, Partial Has Free Count:%d\n", 
			    pKmem->m_nHasUsePage, pKmem->m_nHasFreePage,pKmem->GetSlabNode()->GetPartialCount());

		printf("\n");
	}

	m_VirAllocManager.Tracer();
}

void* CKmemManager::KmemMalloc(int nSizeByte)
{
	__try
	{
		CKmem *pKmem = GetKmem(nSizeByte);
		if(NULL==pKmem)
		{
			return NULL;
		}

		return pKmem->KmemMalloc();
	}
	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))
	{
		return NULL;
	}
}

void CKmemManager::KmemFree(void *p)
{
	/*
	通过p算出属于那个VirAlloc，
	再通过VirAlloc的接口算出页面
	*/
	__try
	{
		VIR_PAGE_HEAD *pPackHead = m_VirAllocManager.GetPageHead(p);
		IF(NULL==pPackHead)
		{
			return ;
		}
		CKmem *pKmem = pPackHead->m_pKmem;

		pKmem->KmemFree(p);
	}
	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))
	{
		return;
	}
}

void CKmemManager::Init()
{
	/*
		计算出nKmemGrainByte大小，然后寻找符合的页面Viralloc
	*/
	char strName[name_slab::KMEM_NAME_SIZE]={0};
	int nKmemGrainByte, nKmemMetaGrainByte;
	CVirAlloc *pVirAlloc;

	for(int i=0; i<name_slab::KMEM_COUNT; ++i)
	{
		nKmemGrainByte = name_slab::KMEM_GRAIN_BYTE[i];
		nKmemMetaGrainByte = CalcSize(nKmemGrainByte);

		pVirAlloc = m_VirAllocManager.GetVirAlloc(nKmemMetaGrainByte);
		INITASSERT(NULL==pVirAlloc);
		INITASSERT(pVirAlloc->GetAllocGrainByte()<nKmemMetaGrainByte);
		
		sprintf(strName, "Kmem %d Byte", nKmemGrainByte);

		m_Kmem[i].Init(strName, nKmemGrainByte, nKmemMetaGrainByte, pVirAlloc);	
	}
}

int CKmemManager::CalcSize(int nSize)
{	
	if( DEBUG_DEFAULT_FALGS & SLAB_RED_ZONE )
	{
		nSize += sizeof(name_slab::BAD_ALIEN_MAGIC);
	}

	nSize = _ALIGN(nSize, __ALIGN_SIZE);

	return nSize;
}

CKmem* CKmemManager::GetKmem(int nSizeByte)
{
	int nIndex = GetKmemIndex(nSizeByte);
	if(-1==nIndex)
	{
		return NULL;
	}

	return &m_Kmem[nIndex];
}

int CKmemManager::GetKmemIndex(int nSizeByte)
{	
	for(int i=0; i<name_slab::KMEM_COUNT; ++i)
	{
		if(nSizeByte<=m_Kmem[i].GetObjectSizeByte())
		{
			return i;
		}
	}

	return -1;
}