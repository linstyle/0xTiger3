#pragma  once
#include "LinuxList.h"
#include "CWindowsSlabDef.h"
//lindp
//2011.7.17 22:54

/*
	页面的属性信息，在DEBUG方面很有帮助
*/

/*
	页面的当前状态
*/
enum PAGE_STATE
{
	STATE_CACHE_LOCK,						//被某个SLAB持有
	STATE_PARTIAL_LOCK,						//在Partial节点中
	STATE_ALL_USER_LOCK,					//全部都被用户持有
};

class CKmem;
struct VIR_PAGE_HEAD 
{
	void Init(int nFreeObject, CKmem *pKmem, void *pFreeList) 
	{
		m_nFreeObject = nFreeObject;
		m_pFreeList = pFreeList;
		m_pKmem = pKmem;
		m_nState = STATE_CACHE_LOCK;
		m_nBadAlienMagic = name_slab::HEAD_BAD_ALIEN_MAGIC;
		INIT_LIST_HEAD(&m_PageListNode);
	}

	void *m_pFreeList;					 //空闲的内存头
	int m_nFreeObject;					 //空闲的object数量
	CKmem *m_pKmem;						 //对应的CKmem

	PAGE_STATE m_nState;			 //debug状态
	unsigned long m_nBadAlienMagic;		 //防止转换出错
	struct list_head m_PageListNode;	 //页面之间的链表节点
};
