#include "StdAfx.h"
#include "CSocketLogic.h"


CSocketLogic::CSocketLogic()
{	

}

void CSocketLogic::TracerMe()
{
	DELAY_CONTINUE(1000*10)
	{
		printf("Notice, CSocketLogic:\n");
		printf("  Send Thread Count:%d\n", m_ListSendActive.GetCount());
		printf("  Recv Thread Count:%d\n", m_ListRecvActive.GetCount());
		printf("  Exception Thread Count:%d\n", m_ListExceptionActive.GetCount());
	}
}


list_head* CSocketLogic::GetListSend()
{
	return &m_ListSendActive.m_ListHead;
}

list_head* CSocketLogic::GetListRecv()
{
	return &m_ListRecvActive.m_ListHead;
}

list_head* CSocketLogic::GetListException()
{
	return &m_ListExceptionActive.m_ListHead;
}

CSocketLogic::~CSocketLogic()
{
}

void CSocketLogic::FlushRecv()
{
	int nResult, nPacketContentLen;
	CIOCPData *pIOCPKey;	
	list_head *pHead, *pEach, *pSafe;

	pHead = GetListRecv();
	
	list_for_each_safe(pEach, pSafe, pHead)
	{
		pIOCPKey = list_entry_offset(pEach, CIOCPData, m_ListRecv);
		if( true==pIOCPKey->SocketGC.IsException() )
		{
			DelRecv(pIOCPKey, true);
			continue;
		}
		
		nResult = pIOCPKey->RecvPacketHead();
		if( -1==nResult )
		{
			/*
				表示空间不够，从链表中删除,并且投递一个接收
			*/					
			ResetActiveRecv(pIOCPKey);
			continue;
		}

		nPacketContentLen = pIOCPKey->InitRecvPacketContent();
		if( -1==nPacketContentLen )
		{
			DelRecv(pIOCPKey, true);			
			continue;
		}
		nResult = pIOCPKey->RecvPacketContent(nPacketContentLen);
		if( -1==nResult )
		{		
			ResetActiveRecv(pIOCPKey);
			continue;
		}

		pIOCPKey->RecvBuffer.ReadBufferFlush(nPacketContentLen+name_msg_packet::PACKET_HEAD_SIZE);
		
		/*
			调试信息
		*/
		++g_NetTracer.m_nRecv;
		++g_NetTracer.m_nRecvSec;		
		pIOCPKey->RecvBuffer.TracerMe();

		if( -1==ExcutePacket(pIOCPKey))
		{
			DelRecv(pIOCPKey, true);
			continue;
		}

		if( pIOCPKey->RecvBuffer.GetUseLength()<=20 )//判断当前有效长度
		{
			ResetActiveRecv(pIOCPKey);		
		}
	}

}

void CSocketLogic::FlushSend()
{
	int nResult;
	CIOCPData *pIOCPKey;
	list_head *pHead, *pEach, *pSafe;

	pHead = GetListSend();
	
	list_for_each_safe(pEach, pSafe, pHead)
	{
		pIOCPKey = list_entry_offset(pEach, CIOCPData, m_ListSend);
		if( true==pIOCPKey->SocketGC.IsException() )
		{
			DelSend(pIOCPKey, true);
			continue;
		}

		nResult = pIOCPKey->FlushSend();
		if( -1==nResult )
		{
			DelSend(pIOCPKey, true);			
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

int CSocketLogic::AddSend(CIOCPData *pIOCPKey)
{			
	/*
		debug, 防止重复
	*/
	assert(false==pIOCPKey->SocketDebugFlag.IsActiveSend() );

	pIOCPKey->SocketGC.Increase();
	pIOCPKey->SocketDebugFlag.SetActiveSend(true);

	m_ListSendActive.Add(&pIOCPKey->m_ListSend);

	printf("Notic, CSocketLogic::AddSend:%x\n", pIOCPKey);

	return 0;
}

int CSocketLogic::AddRecv(CIOCPData *pIOCPKey)
{
	/*
		debug, 防止重复
	*/
	assert(false==pIOCPKey->SocketDebugFlag.IsActiveRecv() );

	pIOCPKey->SocketGC.Increase();
	pIOCPKey->SocketDebugFlag.SetActiveRecv(true);

	m_ListRecvActive.Add(&pIOCPKey->m_ListRecv);

	return 0;
}

void CSocketLogic::AddException(CIOCPData *pIOCPKey, LONG nLastCount)
{
	//debug,防止多次重复添加
	assert(nLastCount>=0);

	pIOCPKey->SocketGC.SetException(true);
	if(0==nLastCount)
	{
		m_ListExceptionActive.Add(&pIOCPKey->m_ListException);
		printf("Notic, CSocketLogic::AddException,%x\n", pIOCPKey);
	}
}

void CSocketLogic::FlushException(CIOCPData *pIOCPKey)
{
	/*
		逻辑层关闭
	*/
	if( -1==ClosePacket(pIOCPKey) )
	{
		printf("Err, CSocketLogic::AddException;ClosePacket,%x\n", pIOCPKey);
	}
	
	DelException(pIOCPKey);
}

void CSocketLogic::DelException(CIOCPData *pIOCPKey)
{
	m_ListExceptionActive.Del(&pIOCPKey->m_ListException);
}

void CSocketLogic::DelSend(CIOCPData *pIOCPKey, bool bException)
{
	assert(true==pIOCPKey->SocketDebugFlag.IsActiveSend());
	
	pIOCPKey->SocketDebugFlag.SetActiveSend(false);
	m_ListSendActive.Del(&pIOCPKey->m_ListSend);
	
	if( true==bException )
	{
		AddException(pIOCPKey, pIOCPKey->SocketGC.Decrease());
	}
	else
	{
		pIOCPKey->SocketGC.Decrease();
	}

//	printf("CNetDriver::DelActive,%x\n", p->m_pIOCPKey);

}

void CSocketLogic::DelRecv(CIOCPData *pIOCPKey, bool bException)
{
	assert(true==pIOCPKey->SocketDebugFlag.IsActiveRecv());
	
	pIOCPKey->SocketDebugFlag.SetActiveRecv(false);
	m_ListRecvActive.Del(&pIOCPKey->m_ListRecv);
	
	if( true==bException )
	{
		AddException(pIOCPKey, pIOCPKey->SocketGC.Decrease());
	}
	else
	{
		pIOCPKey->SocketGC.Decrease();
	}

//	printf("CNetDriver::DelActive,%x\n", p->m_pIOCPKey);
}


void CSocketLogic::ResetActiveRecv(CIOCPData* pIOCPKey)
{
	/*
		重置套接字，判断重新投递是否成功，然后对原来所在队列的处理
	*/
	
	if( -1==pIOCPKey->Recv() )
	{
		DelRecv(pIOCPKey, true);	
	}
	else
	{
		DelRecv(pIOCPKey);
	}
}

int CSocketLogic::ExcutePacket(CIOCPData* pIOCPKey)
{
	return g_NetTransToLogic.ExcutePacket(pIOCPKey);
}

int CSocketLogic::ClosePacket(CIOCPData* pIOCPKey)
{
	return g_NetTransToLogic.ExceptionPacket(pIOCPKey);
}

void CSocketLogic::CloseAllSocket()
{
	//Send队列
	int nResult;
	CIOCPData *pIOCPKey;
	list_head *pHead, *pEach, *pSafe;

	pHead = GetListSend();
	
	list_for_each_safe(pEach, pSafe, pHead)
	{
		/*
			统一发送一个关闭的数据，使一些不在recv队列中的阻塞的套接字返回
		*/
		pIOCPKey = list_entry_offset(pEach, CIOCPData, m_ListSend);
		if( false==pIOCPKey->SocketGC.IsException() )
		{			
			DelSend(pIOCPKey, true);		
		}
	}

	//Recv队列
	pHead = GetListRecv();
	
	list_for_each_safe(pEach, pSafe, pHead)
	{	
		pIOCPKey = list_entry_offset(pEach, CIOCPData, m_ListRecv);
		if( false==pIOCPKey->SocketGC.IsException() )
		{			
			DelRecv(pIOCPKey, true);
		}
	}

}