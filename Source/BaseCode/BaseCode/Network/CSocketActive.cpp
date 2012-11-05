#include "StdAfx.h"
#include "CSocketActive.h" 

initialiseSingleton(CSocketActive);

CSocketActive::CSocketActive():m_VirAlloc(1, 1),m_Kmem(&m_VirAlloc, "SocketActive", sizeof(SOCKET_ACTIVE))
{	

}

void CSocketActive::TracerMe()
{
	DELAY_CONTINUE(1000*10)
	{
		printf("Notice, CSocketActive:\n");
		printf("  %d Send Thread\n", m_ListSendActive.m_nListNumber);
		for(int i=0; i<m_ListSendActive.m_nListNumber; ++i)
		{
			printf("  Thread:%d, Count:%d", i, m_ListSendActive.m_pListCount[i]);
		}
		printf("\n");

		printf("  %d Recv Thread\n", m_ListRecvActive.m_nListNumber);
		for(int i=0; i<m_ListRecvActive.m_nListNumber; ++i)
		{
			printf("  Thread:%d, Count:%d", i, m_ListRecvActive.m_pListCount[i]);
		}
		printf("\n");
	}
}

void CSocketActive::InitRSList(int nNumber)
{
	m_ListSendActive.Init(nNumber);
	m_ListRecvActive.Init(nNumber);
}

list_head* CSocketActive::GetListSend(int nIndex)
{
	return &m_ListSendActive.m_pListActive[nIndex];
}

list_head* CSocketActive::GetListRecv(int nIndex)
{
	return &m_ListRecvActive.m_pListActive[nIndex];
}

int CSocketActive::GetFreeListSend()
{
	int nMin=0;
	for(int i=1; i<m_ListSendActive.m_nListNumber; ++i)
	{
		if( m_ListSendActive.m_pListCount[nMin].GetValue()>m_ListSendActive.m_pListCount[i].GetValue() )
		{
			nMin = i;
		}
	}
	
	return nMin;
}



CSocketActive::~CSocketActive()
{
}

void CSocketActive::FlushRecv(int nIndex)
{
	int nResult, nPacketContentLen;
	CIOCPData *pIOCPKey;
	SOCKET_ACTIVE *pActive;	
	list_head *pHead, *pEach, *pSafe;

	pHead = GetListRecv(nIndex);
	
	list_for_each_safe(pEach, pSafe, pHead)
	{
		pActive = list_entry_offset(pEach, SOCKET_ACTIVE, m_ListRecv);
		pIOCPKey = pActive->m_pIOCPKey;
		if( true==pIOCPKey->SocketGC.IsException() )
		{
			DelRecv(pActive, true);
			continue;
		}
		
		nResult = pIOCPKey->RecvPacketHead();
		if( -1==nResult )
		{
			/*
				表示空间不够，从链表中删除,并且投递一个接收
			*/					
			ResetActiveRecv(pActive);
			continue;
		}

		nPacketContentLen = pIOCPKey->InitRecvPacketContent();
		if( -1==nPacketContentLen )
		{
			DelRecv(pActive, true);			
			continue;
		}
		nResult = pIOCPKey->RecvPacketContent(nPacketContentLen);
		if( -1==nResult )
		{		
			ResetActiveRecv(pActive);
			continue;
		}

		pIOCPKey->RecvBuffer.ReadBufferFlush(nPacketContentLen+name_msg_packet::PACKET_HEAD_SIZE);
		
		/*
			调试信息
		*/
		++g_NetTracer.m_nRecv;
		++g_NetTracer.m_nRecvSec;		
		pIOCPKey->RecvBuffer.TracerMe();

		if( -1==g_NetDriver.ExcutePacket(pIOCPKey))
		{
			DelRecv(pActive, true);
			continue;
		}

		if( pIOCPKey->RecvBuffer.GetUseLength()<=20 )//判断当前有效长度
		{
			ResetActiveRecv(pActive);		
		}
	}

}

void CSocketActive::FlushSend(int nIndex)
{
	int nResult;
	CIOCPData *pIOCPKey;
	SOCKET_ACTIVE *pActive;
	list_head *pHead, *pEach, *pSafe;

	pHead = GetListSend(nIndex);
	
	list_for_each_safe(pEach, pSafe, pHead)
	{
		pActive = list_entry_offset(pEach, SOCKET_ACTIVE, m_ListSend);
		pIOCPKey = pActive->m_pIOCPKey;
		if( true==pIOCPKey->SocketGC.IsException() )
		{
			DelSend(pActive, true);
			continue;
		}

		nResult = pIOCPKey->FlushSend();
		if( -1==nResult )
		{
			DelSend(pActive, true);			
		}
		//printf("FlushSendActive:%x\n", pIOCPKey);
		
		/*
			调试信息
		*/
		pIOCPKey->SendBuffer.TracerMe();
		++g_NetTracer.m_nSend;
		++g_NetTracer.m_nSendSec;
	}
}

void CSocketActive::FlushException(CIOCPData* p)
{
	g_NetDriver.ClosePacket(p);
}

int CSocketActive::AddSend(CIOCPData* p)
{
	SOCKET_ACTIVE *pActive = MallocSocketActive();
	if( NULL==pActive )
	{
		return -1;
	}

	/*
		获取一个发送线程序号
	*/
	int i =GetFreeListSend();

	p->SocketGC.Increase();
	p->bActiveSend = true;
	p->bDebugSendList = i;
	pActive->m_pIOCPKey = p;

	list_add(&pActive->m_ListSend, GetListSend(i));

	++m_ListSendActive.m_pListCount[i];

	return 0;
}

int CSocketActive::AddRecv(CIOCPData* p)
{
	/*
		debug, 防止重复
	*/
	if( true==p->bActiveRecv )
	{
		return 0;
	}

	SOCKET_ACTIVE *pActive = MallocSocketActive();
	if( NULL==pActive )
	{
		return -1;
	}

	pActive->m_pIOCPKey = p;
	list_add(&pActive->m_ListRecv, GetListRecv(p->bDebugRecvList));

	p->SocketGC.Increase();
	p->bActiveRecv = true;

	m_ListRecvActive.Increase(p->bDebugRecvList);

	return 0;
}

void CSocketActive::AddException(CIOCPData *p, LONG nLastCount)
{
	p->SocketGC.SetException(true);
	if(0==nLastCount)
	{
		FlushException(p);
	}
}

void CSocketActive::DelSend(SOCKET_ACTIVE *p, bool bException)
{
	assert(true==p->m_pIOCPKey->bActiveSend);

	CIOCPData* pIOCPKey = p->m_pIOCPKey;
	
	pIOCPKey->bActiveSend  = false;	
	list_del(&p->m_ListSend );
	m_ListSendActive.Decrease(pIOCPKey->bDebugSendList);
	
	if( true==bException )
	{
		AddException(pIOCPKey, pIOCPKey->SocketGC.Decrease());
	}
	else
	{
		pIOCPKey->SocketGC.Decrease();
	}

//	printf("CNetDriver::DelActive,%x\n", p->m_pIOCPKey);

	FreeSocketActive(p);
}

void CSocketActive::DelRecv(SOCKET_ACTIVE *p, bool bException)
{
	assert(true==p->m_pIOCPKey->bActiveRecv);

	CIOCPData* pIOCPKey = p->m_pIOCPKey;
	
	pIOCPKey->bActiveRecv  = false;
	list_del(&p->m_ListRecv );
	m_ListRecvActive.Decrease(pIOCPKey->bDebugRecvList);
	
	if( true==bException )
	{
		AddException(pIOCPKey, pIOCPKey->SocketGC.Decrease());
	}
	else
	{
		pIOCPKey->SocketGC.Decrease();
	}

//	printf("CNetDriver::DelActive,%x\n", p->m_pIOCPKey);

	FreeSocketActive(p);
}

SOCKET_ACTIVE* CSocketActive::MallocSocketActive()
{
	void *p = m_Kmem.KmemMalloc();
	if( NULL==p )
	{
		return NULL;
	}

	SOCKET_ACTIVE *pActive = new(p)SOCKET_ACTIVE;

	return pActive;
}

void CSocketActive::FreeSocketActive(SOCKET_ACTIVE* p)
{
	/*
		根据C++对象原则，主动调用析构函数
	*/
	p->~SOCKET_ACTIVE();
	m_Kmem.KmemFree((void*)p);	
}

void CSocketActive::ResetActiveRecv(SOCKET_ACTIVE* p)
{
	CIOCPData *pIOCPKey = p->m_pIOCPKey;

	DelRecv(p);
	pIOCPKey->Recv();
}
