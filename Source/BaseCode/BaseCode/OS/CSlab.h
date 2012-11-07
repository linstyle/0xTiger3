/*  作者：		林东平 lin_style@foxmail.com                                                        
	创建时间：	2010/06/29                                                        
    功能说明：  一个快速分配、释放不同大小内存类型的对象池中间层，一些原理参考linux中的slab,slub.名字维持slab
	要求:       32位机器测试没问题，虽然也针对了64位设计，但是没正式测过；
				只能在小端机器上运行


	可以调节的参数：是否开启坏区检测(默认关闭)，
					是否开启CPU亲缘(默认打开)
					是否开启多余页面回收(默认关闭)
					是否预分配内存(默认关闭)
*/

#pragma once


#include "Singleton.h"
#include "OSOther.h"
#include "CVirAlloc.h"
#include "MTASSERT.h"
/*
	CONFIG_SLAB_DEBUG	:总开关
	DEBUG_DEFAULT_FALGS :需要跟踪的具体DEBUG标签
*/
#define CONFIG_SLAB_DEBUG


#ifdef CONFIG_SLAB_DEBUG

	#define DEBUG_DEFAULT_FALGS (SLAB_RED_ZONE | CPU_RUN_ID | GC_PARTIAL_PAGE )

#else
	
	#define DEBUG_DEFAULT_FALGS 0

#endif 

/*
	需要调试的DEBUG标签，递增顺序为
	0x1,0x2,0x4,0x8
	0x10,0x20,0x40,0x80
	0x100,0x200,0x300,0x400
	...
*/
#define SLAB_RED_ZONE 0x1	//0x1	开启坏区检测
#define CPU_RUN_ID 0x2		//0x2	开启多核CPU缓存,即尽量保证切换时，在使用上不会出现脏的页面需要重刷
#define GC_PARTIAL_PAGE 0x4 //0x4	开启多余页面回收


namespace name_slab
{
	/*
		KMEM_COUNT决定每个Kmem支持的对象大小,单位为byte
		计算方式为2^KMEM_COUNT次方.
	*/
	const int KMEM_GRAIN_BYTE[] = {4, 8, 16, 32, 64, 128, 256, 512, 
		                           1024,  1024*2,  1024*4, 1024*8, 1024*16
								   };
	const int KMEM_COUNT = sizeof(KMEM_GRAIN_BYTE)/sizeof(*KMEM_GRAIN_BYTE);
	/*
		缓冲页面的保留数.当程序检测到空余页面大于MIN_PARTIAL时，对多余的页面进行释放给系统
	*/
	const unsigned int BAD_ALIEN_MAGIC = 0x01020304;
	/*
		每个kmem的名字长度，方便调试
	*/
	const int KMEM_NAME_SIZE = 24;
	/*
		保留空闲内存的百分比，多余的将释放给操作系统
		50:百分之五十
	*/
	const int MIN_PARTIAL_PER = 50;  
};

class CBadAlienMagic
{
public:
	CBadAlienMagic()
	{
		m_nOffset=0;
	}

	unsigned int GetBadAlienMagic()
	{
		++m_nOffset;
		return name_slab::BAD_ALIEN_MAGIC+m_nOffset;
	}
	unsigned int m_nOffset;
};

/*	
	数据协议如下，red校验放在最后，所以在verify的时候直接从后面开始计算
    |..............|   |..............|
	|list|object....red|list|object....red|list|object....red
*/
class CSlabCache
{
public:
	CSlabCache();

private:

public:
	unsigned int m_nOffset;
	void* m_pFreeList;
	VIR_PAGE_HEAD *m_pPage;

private:
	
	
};

class CSlabNode
{
public:
	CSlabNode();
	void PushPageHead(VIR_PAGE_HEAD* p);
	VIR_PAGE_HEAD* PopPageHead();
	void PopPageHead(VIR_PAGE_HEAD* p);	
	unsigned long GetPartialCount();

private:
	void __PushPageHead(VIR_PAGE_HEAD* p);
	VIR_PAGE_HEAD* __PopPageHead();	

public:


private:
	unsigned long m_nPartialListCount;
	list_head m_PartialList;
	CFastLock m_FastLock;
};


class CKmem
{
public:
	~CKmem();

	/*
		strKmemName:Kmem的名字，方便调试
		nKmemSizeKB:Kmem的大小
		nAllocMB/nAllocGrainKB:   Kmem包含的alloc初始化参数
	*/
	void Init(const char* strKmemName, int nObjectSizeByte, int nObjectMetaSizeByte, CVirAlloc* pVirAlloc);

	void* KmemMalloc();
	void KmemFree(void *p);

	CSlabNode* GetSlabNode();
	CVirAlloc* GetVirAlloc();

	int GetObjectSizeByte();
	int GetObjectMetaSizeByte();
	const char*GetKmemName();
	unsigned long GetMinPartial();
	void SetMinPartial(int nMinPartial);

private:	
	CSlabCache *GetCpuSlab();
	CSlabCache *GetSlab(int nIndex);

	void InitMemoryList(void *p);
	/*
		MallocPage:当freelist中无数据的时候，从节点或者邻居中获取新页面
	*/
	void* MallocPage(CSlabCache *s);
	/* 
		提供CSlabCache参数，防止执行中线程被CPU切换
	*/
	void* MallocPartialNode(CSlabCache *s);
	void* MallocOSPage(CSlabCache *s);

	bool FreeLocal(VIR_PAGE_HEAD* pPage,void *p);
	void FreeNode(VIR_PAGE_HEAD* pPage, void *p);

	/*
		p:一个完整的object起始地址，返回magic的地址
	*/
	void* GetBadAlienMagicAddr(void *p);
public:
	/*
		debug需要的一些调试(都为有效分配统计)
		m_nHasUsePage: 使用了多少页面
		m_nHasFreePage: 释放了多少次页面
		m_nCountMalloc: 进行了多少次分配
		m_nCountMallocPartial: 从邻居中分配了多少次
		m_nCountFree:   进行了多少次释放
		m_nBadAlienMagic:内存是否被破坏的数字校验
	*/
	unsigned long m_nHasUsePage;
	unsigned long m_nHasFreePage;
	unsigned long m_nCountMalloc;
	unsigned long m_nCountMallocPartial;
	unsigned long m_nCountFree;
	unsigned long m_nBadAlienMagic;

private:
	/*
		m_nObjectSizeByte :分配每个对象的大小
		m_nObjectMetaSizeByte: 包括Meta的数据
	*/	
	int m_nObjectSizeByte;
	int m_nObjectMetaSizeByte;
	char m_szName[name_slab::KMEM_NAME_SIZE];

	CSlabCache *m_pSlabCache;
	CSlabNode  m_SlabNode;
	CVirAlloc  *m_pWinVirAlloc;

	/*
		m_nMinPartial:大于此值的后的页面都被释放给系统
	*/
	unsigned long m_nMinPartial;
	int n_mCPUNumberOf;
	CFastLock m_FastLock;
};

/*
	负责打印输出各个CKmem的使用情况
*/
class CKmemManager:public Singleton<CKmemManager>
{
public:
	void TracerMe();
	void Tracer();

	CKmemManager();
	
	void* KmemMalloc(int nSizeByte);
	void KmemFree(void *p);

private:
	void Init();

	/*
		返回包括meta data以及字节对齐后的大小
	*/
	int CalcSize(int nSize);
	CKmem* GetKmem(int nSizeByte);
	int GetKmemIndex(int nSizeByte);

public:

private:
	CKmem m_Kmem[name_slab::KMEM_COUNT];
	CVirAllocManager m_VirAllocManager;

};

#define g_KmemManager CKmemManager::getSingleton()