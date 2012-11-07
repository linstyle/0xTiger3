/*  作者：		林东平                                                              
	创建时间：	2010/06/29                                                         
    功能说明：  对Windows调拨器的一个封装
				该调拨器主要负责了地址上的预定和物理调拨器的释放
				每次默认单位颗粒为一页(跟机器位数相关,32位为4KB）
				对于大于单位颗粒的申请，在页面头里做好信息记录，剩余走的逻辑一样
*/
#include "GlobalType.h"
#include "Singleton.h"
#include "LinuxList.h"
#include "CLock.h"

#include <vector>

using namespace std;

namespace name_viralloc
{
	//记录内存页是否释放、申请的bitmap
	const int BIT_MAP_SIZE = sizeof(long);

	/*
		 ALLOC_GRAIN_SIZE_KB,ALLOC_MEM_SIZE_MB表示对应的颗粒度可以分配的内存空间大小
		 const int ALLOC_GRAIN_SIZE_KB[]={n};n表示可支持的最大对象，单位kb
		 const int ALLOC_MEM_SIZE_MB[]=  {n};n表示分配的内存最大, 单位mb
	*/	
	const int ALLOC_GRAIN_SIZE_KB[]={16,1024*5};
	const int ALLOC_MEM_SIZE_MB[]=  {350,250};
	const int ALLOC_COUNT = sizeof(ALLOC_GRAIN_SIZE_KB)/sizeof(*ALLOC_GRAIN_SIZE_KB);

	//每个颗粒度的大小
	const int ALLOC_GRAIN_KB = __PAGE_SIZE_BYTE/1024;
	const int ALLOC_GRAIN_BYTE = ALLOC_GRAIN_KB*1024;
};

/*
	页面的当前状态
*/
enum PAGE_STATE
{
	SLAB_LOCK,		//被某个SLAB持有
	PARTIAL_LOCK,   //在Partial节点中
	OS_LOCK,		//归还给VirAlloc
};
/*
	维护分配每个颗粒度大小的表头
*/
class CKmem;
struct VIR_PAGE_HEAD
{
	void Init() 
	{
		m_nHasUseObject = 0;
		m_pFreeList = NULL;
		m_pKmem = NULL;
		m_nState = OS_LOCK;
		INIT_LIST_HEAD(&m_PageNext);
	}
	int m_nHasUseObject;
	CKmem *m_pKmem;	
	void *m_pFreeList;

	PAGE_STATE m_nState;			//debug方便
	struct list_head m_PageNext;
};


/*
	地址的一个查询对照表
	算出属那个颗粒度的索引
*/
class CVirPageAddress
{
public:
	CVirPageAddress();
	struct ALLOC_ADDRESS
	{
		ALLOC_ADDRESS(unsigned long Start=0, unsigned long End=0)
		{
			m_Start = Start;
			m_End = End;
		}
		unsigned long m_Start;
		unsigned long m_End;
	};
	void Add(unsigned long Start, unsigned long End);
	int GetAddressIndex(unsigned long nAddress);

private:

public:

private:
	vector<ALLOC_ADDRESS> m_AllocAddress;
};

/*
	一个二维的页面偏移表
	定义好起始地址、列长度，最后算出所在的行数
*/
class CVirPageTable
{
public:
	void Init(unsigned long Start, unsigned long Offset, unsigned long End);

	long ToPageOffset(void* pUnknowStart);

private:

public:

private:
	unsigned long m_Start;	//起始地址
	unsigned long m_Offset;	//列长度
	unsigned long m_End;	//最大地址
};

/*
	地址的页面头维护
*/
class CVirPageHead
{
public:
	~CVirPageHead();
	void Init(long nPageCount);
	VIR_PAGE_HEAD* GetPageHead(long nPageIndex);

private:

public:

private:
	VIR_PAGE_HEAD *m_pVirPageHead;
	long m_nPageCount;

};

class CVirPageAssis
{
public:	
	void InitPageHead(long nPageCount);
	void InitPageTable(unsigned long Start, unsigned long Offset, unsigned long End);

	/*
		通过内存地址返回对应的页面或者偏移量
	*/
	VIR_PAGE_HEAD* GetPageHead(void *p);
	long GetPageHeadIndex(void* p);
private:
	

public:

private:
	CVirPageHead m_VirPageHead;
	CVirPageTable m_VirPageTable;
};

class CVirAlloc
{
public:
	CVirAlloc();
	~CVirAlloc();
	
	void Init(long nAllocMB, long nAllocGrainKB);
	/*
		MallocPage	: 返回一个空闲页
		FreePage	: 释放当前地址所在的整个页面
	*/
	VIR_PAGE_HEAD* MallocPage();
	void  FreePage(VIR_PAGE_HEAD *pPage);

	/*
		通过一个地址找到该地址所在的页面
	*/
	VIR_PAGE_HEAD* GetPageHead(void *p);

	/*
		存取接口
	*/
	long GetAllocMemMB();
	long GetAllocGrainByte();
	long GetPageCount();
	long GetFreePageCount();
	long GetUsePageCount();
	void* GetAddressBegin();
	void* GetAddressEnd();

private:
	void InitAllocMem();
	void InitVirPageAssis();
	void InitPageOffset();
	
	long __MallocPage();
	long __FreePage(long nOffset);

	/*
		函数功能:用一个位标识，来说明当前操作的空间是否有效
		nOffset	:页面的数组下标，从0开始
	*/
	void SetDebugFlag(long nOffset, long nFlag);
	long  GetDebugFlag(long nOffset);

public:
	/*
		 一个转换地址表和表头
	*/
	CVirPageAssis m_VirPageAssis;

private:
	/*
		m_pAddressBegin	: 起始地址
		m_PageOffset: 存放空闲的偏移量
	*/
	void* m_pAddressBegin;
	void* m_pAddressEnd;
	vector<long> m_PageOffset;

	/*
		m_pDebugFlag; : bit为单位，检测是否被重复释放或获取。0表释放，1表示已获取
	*/
	long *m_pDebugFlag;
	long m_nUsePageCount;

	/*
		该alloc里面允许的最大内存和颗粒度
	*/
	long m_nAllocMemMB;
	long m_nAllocMemKB;
	unsigned long m_nAllocMemByte;

	long m_nAllocGrainKB;
	unsigned long m_nAllocGrainByte;

	CFastLock m_FastLock;
};

class CVirAllocManager
{
public:
	CVirAllocManager();

	void TracerMe();
	void Tracer();

	CVirAlloc* GetVirAlloc(long nSizeByte);
	VIR_PAGE_HEAD* GetPageHead(void *p);

private:
	void Init();
	long GetVirAllocIndex(long nSizeByte);

public:

private:
	CVirPageAddress m_VirPageAddress;
	CVirAlloc m_VirAlloc[name_viralloc::ALLOC_COUNT];
};

