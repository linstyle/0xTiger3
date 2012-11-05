#pragma once
//lindp
//2011.7.17
#include "GlobalType.h"
#include "CLock.h"
#include "LinuxList.h"
#include "CVirPageHead.h"
#include "CWindowsSlabDef.h"
#include "CLog2.0.h"

/*
	Slab缓冲页面，跟CPU挂钩
*/
class CSlabCache
{
public:
	CSlabCache();
	~CSlabCache();

private:

public:
	VIR_PAGE_HEAD *m_pPage;

private:

};

/*
	Slab邻居节点页面管理
*/
class CSlabPartial
{
public:
	CSlabPartial();
	~CSlabPartial();
	void PushPageHead(VIR_PAGE_HEAD* p);

	/*
		VIR_PAGE_HEAD* PopPageHead():		返回某个页面，并且删除
		void PopPageHead(VIR_PAGE_HEAD* p):	把某个页面从中删除
	*/
	VIR_PAGE_HEAD* PopPageHead();
	void PopPageHead(VIR_PAGE_HEAD* p);	

	//返回当前所持有的页面数
	unsigned long GetPartialCount();

private:
	/*
		数据容器测试，封装一个接口
	*/
	void __PushPageHead(VIR_PAGE_HEAD* p);
	VIR_PAGE_HEAD* __PopPageHead();	

public:


private:
	unsigned long m_nPartialListCount;
	list_head m_PartialList;
};


class CKmem
{
public:
	CKmem();
	~CKmem();

	void Init(long nMallocObjectSizeByte);
	//打印出内部信息
	void TraceMe();

	void* Kmalloc();
	void  Kfree(void *p);

private:
	//初始化DEBUG变量
	void InitD();
	/* 
		MallocPage依次调用MallocPartialNode,MallocOSPage。而之前的CSlabCache
		已经确定。如果这个时候发生CPU切换，还是会造成脏缓存
		在linux中有个中断的函数local_irq_save，防止CPU中断切换,暂时不知道windows下用什么实现
	*/
	void* MallocPage(CSlabCache *s);
	VIR_PAGE_HEAD* MallocPartialNode();
	VIR_PAGE_HEAD* MallocOSPage();
	//对从OS中新申请的页面初始化
	void InitNewOSPage(void *p);
	//对从OS中新申请的页面内存初始化
	void InitMemeryOSPage(VIR_PAGE_HEAD *pPage);

	/*
		Free也有Malloc同样的中断问题
	*/
	bool FreeNode(VIR_PAGE_HEAD* pPage,void *p);
	bool FreePartialNode(VIR_PAGE_HEAD* pPage, void *p);
	void FreeOSPage(VIR_PAGE_HEAD* pPage);
	/*
		辅助函数
	*/
	CSlabCache *GetCpuSlab();
	CSlabCache *GetCpuSlab(int nIndex);

public:
	/*
		debug观察变量
	*/
	//分配成功、失败了多少次
	//释放成功、失败了多少次
	//从Partial分配了多少次
	int m_nDMallocCount;
	int m_nDFreeCount;
	int m_nDPartialMallocCount;
	int m_nDPartialFreeCount;

private:
	CSlabCache *m_pSlabCache;
	CSlabPartial m_SlabPartial;

	int m_nCPUNumberOf;

	CFastLock m_FastLock;
	/*
		很重要的计量变量。
		m_nSingleObjectSizeByte:表示本kmem中，支持的单个对象大小
		m_nSinglePageSizeKB:每次申请的单个页面大小。根据初始m_nMallocObjectSizeByte值，自动判断
		m_nSingleMaxCountObject:根据前面2个值，算出单个页面能分配多少个对象
	*/
	long m_nSingleObjectSizeByte;
	long m_nSinglePageSizeKB;	
	long m_nSingleMaxCountObject;
};
