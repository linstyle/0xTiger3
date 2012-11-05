/*  
	作者：		林东平                                                              
	创建时间：	2010/08/20                                                         
    功能说明：  被激活的终端工作集合
*/
#pragma once

#include "GlobalType.h"
#include "Singleton.h"
#include "CLock.h"
#include "CSlab.h"
#include "LinuxList.h"
#include "CNetObject.h"
#include "CNetTracer.h"
#include "CNetDriver.h"

/*
	每个被激活套接字的结构
*/
struct SOCKET_ACTIVE
{
	SOCKET_ACTIVE()
	{		
		m_pIOCPKey = NULL;
		INIT_LIST_HEAD(&m_ListSend);
		INIT_LIST_HEAD(&m_ListRecv);
	}
	~SOCKET_ACTIVE()
	{
	
	}

	CIOCPData *m_pIOCPKey;
	struct list_head m_ListSend;
	struct list_head m_ListRecv;
};

/*
	对链表的一个小封装，加了些计数器什么的
*/
struct LIST_ACTIVE
{
	LIST_ACTIVE()
	{
		m_nListCount= 0 
		INIT_LIST_HEAD(m_pListNext);
	}

	void Increase()
	{
		++m_nListCount;
	}
	void Decrease()
	{
		--m_nListCount;
	}

	unsigned int m_nListCount;	
	struct list_head *m_pListNext;
};

class CSocketActive:public Singleton<CSocketActive>
{
public:
	CSocketActive();
	~CSocketActive();

	void TracerMe();
	
	void InitRSList(int nNumber);
	list_head* GetListSend(int nIndex);
	list_head* GetListRecv(int nIndex);

	void FlushRecv(int nIndex);
	void FlushSend(int nIndex);
	void FlushException(CIOCPData* p);

	int AddRecv(CIOCPData* p);
	int AddSend(CIOCPData* p);
	void AddException(CIOCPData* p, LONG nLastCount);

	void FreeSocketActive(SOCKET_ACTIVE* p);

private:
	void DelSend(SOCKET_ACTIVE *p, bool bException=false);
	void DelRecv(SOCKET_ACTIVE *p, bool bException=false);
	void DelExcepion(SOCKET_ACTIVE *p, bool bException=false);
	void ResetActiveRecv(SOCKET_ACTIVE* p);

	int GetFreeListSend();

	/*
		对象池封装
	*/
	SOCKET_ACTIVE* MallocSocketActive();
	
public:	
	LIST_ACTIVE m_ListSendActive;
	LIST_ACTIVE m_ListRecvActive;
	
private:
	/*
		激活对象缓冲池
	*/
	CVirAlloc m_VirAlloc;
	CKmem m_Kmem;	
};

