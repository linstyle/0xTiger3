#include "CWindowsSlab.h"
#include <stdio.h>
#include <assert.h>
CSlabCache::CSlabCache()
{
	m_pPage = NULL;
}

/*
	CSlabNode
*/
CSlabPartial::CSlabPartial()
{
	INIT_LIST_HEAD(&m_PartialList);
	m_nPartialListCount = 0;
}

CSlabPartial::~CSlabPartial()
{
	VIR_PAGE_HEAD *pPage;

	while ( NULL!= (pPage=PopPageHead()) )
	{
		free(pPage);
	}
}

VIR_PAGE_HEAD* CSlabPartial::PopPageHead()
{
	VIR_PAGE_HEAD* p;

	p = __PopPageHead();

	return p;
}

void CSlabPartial::PopPageHead(VIR_PAGE_HEAD* p)
{
	list_del(&p->m_PageListNode);	
	--m_nPartialListCount;
}

void CSlabPartial::PushPageHead(VIR_PAGE_HEAD* p)
{		
	__PushPageHead(p);
}

unsigned long CSlabPartial::GetPartialCount()
{
	return m_nPartialListCount;
}

VIR_PAGE_HEAD* CSlabPartial::__PopPageHead()
{
	list_head *pListIte;
	VIR_PAGE_HEAD *pPageHead;

	pListIte = list_get(&m_PartialList);
	if(NULL==pListIte)
	{
		return NULL;
	}

	pPageHead = list_entry_offset(pListIte, VIR_PAGE_HEAD, m_PageListNode);


	--m_nPartialListCount;
	return pPageHead;
}

void CSlabPartial::__PushPageHead(VIR_PAGE_HEAD* p)
{	
	++m_nPartialListCount;
	list_add(&p->m_PageListNode,  &m_PartialList);
}

CKmem::~CKmem()
{
	delete []m_pSlabCache;
}

void CKmem::Init(long nMallocObjectSizeByte)
{
	//获取CPU个数
	SYSTEM_INFO sinf;
	GetSystemInfo(&sinf);

	int nCPUNumberOf=0;

	nCPUNumberOf = sinf.dwNumberOfProcessors;

	m_nCPUNumberOf = nCPUNumberOf;

	//开辟SlabCache
	m_pSlabCache = new CSlabCache[m_nCPUNumberOf];

	//设置page值
	m_nSingleObjectSizeByte = nMallocObjectSizeByte;
	//设置页面数为m_nMallocObjectSizeByte的100倍
	long nSumPageSizeByte = name_slab::CalcSingleObjectSize(nMallocObjectSizeByte)*100 + sizeof(VIR_PAGE_HEAD);
	m_nSinglePageSizeKB = (long)(nSumPageSizeByte/ 1024.0 + 1); //防止出现nSumPageSizeByte过小情况
	m_nSinglePageSizeKB = _ALIGN(m_nSinglePageSizeKB, __ALIGN_SIZE_PAGE);

	m_nSingleMaxCountObject = ( m_nSinglePageSizeKB*1024-sizeof(VIR_PAGE_HEAD) ) / name_slab::CalcSingleObjectSize(nMallocObjectSizeByte);

	//初始化第一个page
	for (int i=0; i<m_nCPUNumberOf; ++i)
	{
		MallocPage(&m_pSlabCache[i]);
	}

	InitD();
}

void CKmem::InitD()
{
	m_nDMallocCount = 0;
	m_nDFreeCount = 0;
	m_nDPartialMallocCount = 0;
	m_nDPartialFreeCount = 0;
}

void CKmem::TraceMe()
{
	printf("m_nSingleObjectSizeByte:%d, m_nPageSizeKB:%d, m_nMaxCountObject:%d\n",
				m_nSingleObjectSizeByte, m_nSinglePageSizeKB, m_nSingleMaxCountObject);
	printf("MallocCount:%d, FreeCount:%d,Malloc Elapse Memery(MB):%f\n", m_nDMallocCount, m_nDFreeCount,
		  m_nDMallocCount*m_nSingleObjectSizeByte/1024.0/1024.0);
	printf("PartialMallocCount:%d, PartialFreeCount:%d\n", m_nDPartialMallocCount, m_nDPartialFreeCount);
	printf("PartialCount:%d\n", m_SlabPartial.GetPartialCount());
	printf("-----------------------------\n");
}

void* CKmem::Kmalloc()
{
	m_FastLock.Lock();

	CSlabCache *s = GetCpuSlab();
	if(NULL==s->m_pPage->m_pFreeList)
	{
		assert(0==s->m_pPage->m_nFreeObject);
		s->m_pPage->m_nState = STATE_ALL_USER_LOCK;
		if(NULL==MallocPage(s))
		{
			m_FastLock.UnLock();
			return NULL;
		}
	}

	void *p = s->m_pPage->m_pFreeList;
	s->m_pPage->m_pFreeList = (void*)(*(long*)p);
	--s->m_pPage->m_nFreeObject;
	
	++m_nDMallocCount;

	m_FastLock.UnLock();

	return p;
}

void  CKmem::Kfree(void *p)
{
	if( NULL==p )
	{
		printf("Err, CKmem::KmemFree:NULL==p\n");
		return;
	}

	/*
		1.判断是否和当前页面相等，相等则释放
		2.不相等，则计算是否其他slab
		3.到pagehead,插入到邻居节点
	*/
	VIR_PAGE_HEAD* pPage = name_slab::OffsetPageHead(p);
	if( NULL==pPage )
	{
		printf("Err, CKmem::KmemFree:NULL==pPage\n");
		return;
	}

	if( DEBUG_SLAB_FALGS & SLAB_RED_ZONE )
	{		
		if( name_slab::HEAD_BAD_ALIEN_MAGIC!=name_slab::OffsetBadMagic(p) )
		{
			printf("Err, CKmem::FreeLocal:m_nBadAlienMagic\n");
			return;
		}
	}

	m_FastLock.Lock();

	if(false==FreeNode(pPage,p))
	{
		//失败++
	}
	
	++m_nDFreeCount;
	m_FastLock.UnLock();
}

bool CKmem::FreeNode(VIR_PAGE_HEAD* pPage,void *p)
{
	++pPage->m_nFreeObject;

	//说明不在当前slab中
	if (STATE_CACHE_LOCK!=pPage->m_nState)
	{
		return FreePartialNode(pPage, p);
	}

	//直接插入
	*(long*)p = (long)pPage->m_pFreeList;
	pPage->m_pFreeList = p;

	return true;
}

bool CKmem::FreePartialNode(VIR_PAGE_HEAD* pPage, void *p)
{
	void *pPrior = pPage->m_pFreeList;

	*(long*)p = (long)pPage->m_pFreeList;
	pPage->m_pFreeList = p;

	/*
		该页面的第一次释放，加入到partial
	*/
	if( NULL==pPrior )
	{
		pPage->m_nState = STATE_PARTIAL_LOCK;
		m_SlabPartial.PushPageHead(pPage);
	}

	++m_nDPartialFreeCount;

	FreeOSPage(pPage);

	return true;
}

void CKmem::FreeOSPage(VIR_PAGE_HEAD* pPage)
{
	if( !(DEBUG_SLAB_FALGS & SLAB_GC_PARTIAL) )
	{
		return;
	}

	if (pPage->m_nFreeObject==m_nSingleMaxCountObject
		&& m_SlabPartial.GetPartialCount() >= name_slab::FREE_KEEP_PARTIAL )
	{
		m_SlabPartial.PopPageHead(pPage);
		free(pPage);
	}
	
}

void* CKmem::MallocPage(CSlabCache *s)
{
	/*
		1.如果为空，则寻找邻居节点
					如果邻居节点为空，申请内存
		2.如果不为空，直接操作链表返回
	*/	
	VIR_PAGE_HEAD* pPage;

	pPage=MallocPartialNode();
	if( NULL==pPage )
	{
		pPage=MallocOSPage();
		if( NULL==pPage )			
		{
			printf("Err, CKmem::MallocPage NULL==MallocOSPage.......\n");
			return NULL;
		}
	}

	assert(STATE_ALL_USER_LOCK != pPage->m_nState);
	pPage->m_nState = STATE_CACHE_LOCK;

	s->m_pPage = pPage;

	return s->m_pPage->m_pFreeList;	
}

VIR_PAGE_HEAD* CKmem::MallocPartialNode()
{
	VIR_PAGE_HEAD *pPage = m_SlabPartial.PopPageHead();
	if (NULL==pPage)
	{
		return NULL;
	}
	
	assert(STATE_PARTIAL_LOCK==pPage->m_nState);
	++m_nDPartialMallocCount;
	return pPage;
}

VIR_PAGE_HEAD* CKmem::MallocOSPage()
{
	void *p = malloc(m_nSinglePageSizeKB*1024);
	if (NULL==p)
	{
		return NULL;
	}

	InitNewOSPage(p);

	return (VIR_PAGE_HEAD *)p;
}

void CKmem::InitNewOSPage(void *p)
{
	//先拆出一个头
	VIR_PAGE_HEAD *pPage = (VIR_PAGE_HEAD*)p;

	pPage->Init(m_nSingleMaxCountObject, this, (void*)( (long)p+sizeof(VIR_PAGE_HEAD) ) );
	InitMemeryOSPage(pPage);
}

void CKmem::InitMemeryOSPage(VIR_PAGE_HEAD *pPage)
{
	void *p = pPage->m_pFreeList;

	for (int i=0; i<m_nSingleMaxCountObject; ++i)
	{	
		//容易出错,2个memcpy的是CalcSingleObjectHeadSize大小，做个assert
		int nVerifyLen=0;
		if( DEBUG_SLAB_FALGS & SLAB_RED_ZONE )
		{
			memcpy(p, &name_slab::HEAD_BAD_ALIEN_MAGIC, sizeof(name_slab::HEAD_BAD_ALIEN_MAGIC) );
			p = (void*)( (long)p + sizeof(name_slab::HEAD_BAD_ALIEN_MAGIC) );
			nVerifyLen += sizeof(name_slab::HEAD_BAD_ALIEN_MAGIC);
		}

		memcpy(p, &pPage, sizeof(pPage));
		p =(void*)( (long)p + sizeof(pPage) );
		nVerifyLen += sizeof(pPage);

		assert( nVerifyLen==name_slab::CalcSingleObjectHeadSize() );

		//记录的是下一个对象内存地址，所以要加上下一个对象内存地址的头部
		*(long*)p = (long)p + m_nSingleObjectSizeByte+name_slab::CalcSingleObjectHeadSize();
		p = (void*)( (long)p + m_nSingleObjectSizeByte ) ;
	}

	p = (void*)( (long)p - m_nSingleObjectSizeByte ) ;
	*(long*)p = NULL;

	//第一个对象跳过headsize
	pPage->m_pFreeList = (void*)( (long)pPage->m_pFreeList+name_slab::CalcSingleObjectHeadSize());
}

CSlabCache *CKmem::GetCpuSlab()
{
	int i=name_slab::GetCPURunId();
	if( DEBUG_SLAB_FALGS & SLAB_CPU_RUN_ID )
	{
		//printf("CKmem::GetCpuSlab:%d\n", i);
	}
	else
	{
		i = 0;
	}
	return &m_pSlabCache[i];
}

CSlabCache *CKmem::GetCpuSlab(int nIndex)
{
	assert(nIndex<m_nCPUNumberOf || nIndex<0);
	if (nIndex>m_nCPUNumberOf || nIndex<0)
	{
		return NULL;
	}
	return &m_pSlabCache[nIndex];
}

namespace name_slab
{
	CKmem Kmem[sizeof(KmemMallocByte)/sizeof(*KmemMallocByte)];
}

void name_slab::kmem_init()
{
	int nKmemCount = sizeof(name_slab::KmemMallocByte)/sizeof(*name_slab::KmemMallocByte);

	for (int i=0; i<nKmemCount; ++i)
	{
		name_slab::Kmem[i].Init(name_slab::KmemMallocByte[i]);
	}
}

void* name_slab::kmem_malloc(int nSizeByte)
{
	int nIndex = kmem_index(nSizeByte);
	if(-1==nIndex)
	{
		return NULL;
	}

	CKmem *pKmem = &Kmem[nIndex];
	if (NULL==pKmem)
	{
		return NULL;
	}

	return pKmem->Kmalloc();
}

void name_slab::kmem_free(void *p)
{
	VIR_PAGE_HEAD *pPage = OffsetPageHead(p);
	if (NULL==pPage)
	{
		printf("Err, name_slab::kmem_free,NULL==pPage\n");
		return;
	}

	CKmem *pKmem = pPage->m_pKmem;
	if(NULL==pKmem)
	{
		printf("Err, name_slab::kmem_free,NULL==pKmem\n");
		return ;
	}

	pKmem->Kfree(p);
}

int name_slab::kmem_index(int nSizeByte)
{
	int nKmemCount = sizeof(name_slab::KmemMallocByte)/sizeof(*name_slab::KmemMallocByte);

	for (int i=0; i<nKmemCount; ++i)
	{
		if (KmemMallocByte[i]>=nSizeByte)
		{
			return i;
		}
	}
	
	return -1;
}

void name_slab::kemem_traceme()
{
	int nKmemCount = sizeof(name_slab::KmemMallocByte)/sizeof(*name_slab::KmemMallocByte);

	printf("		Kmem TraceMe:\n");
	for (int i=0; i<nKmemCount; ++i)
	{
		printf("KmemID:%d\n", i);
		name_slab::Kmem[i].TraceMe();
	}
}