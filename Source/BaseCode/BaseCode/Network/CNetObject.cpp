#include "StdAfx.h"
#include "CNetObject.h"
#include "CNetDriver.h"

CIOCPData::CIOCPData()
{
	Init();
}

CIOCPData::~CIOCPData()
{
	closesocket(nSocket);
	nEvent = IOCP_DEBUG_FREE;
}

void CIOCPData::Init()
{
	INIT_LIST_HEAD(&m_ListSend);
	INIT_LIST_HEAD(&m_ListRecv);
	INIT_LIST_HEAD(&m_ListAllSocket);

	nEvent = IOCP_DEBUG_NEW;
	memset(&OverlappedRecv, 0, sizeof(OverlappedRecv));
}

int CIOCPData::FlushSend()
{
	int nResult;
	
	nResult = FlushSendSub();
	if( -1==nResult )
	{
		return -1;
	}

	nResult = FlushSendSub();
	if( -1==nResult )
	{
		return -1;
	}

	return nResult;
}

int CIOCPData::FlushSendSub()
{
	int nResult;
	int nLen=0;
	const char *pSendBuffer;

	pSendBuffer = SendBuffer.GetReadBuffer(&nLen);

	nResult = CSocketAPI::SendnTCP(nSocket, pSendBuffer, nLen);
	if( -1==nResult )
	{
		printf("Err, CIOCPData::Flush1():ErrID(%d)\n", WSAGetLastError());
		return -1;
	}
	
	SendBuffer.ReadBufferFlush(nResult);
	if( nResult<nLen ) /*调试，发送不完全，等待下回*/
	{				
		//printf("Warning, this:%x, CIOCPData::Flush1(), nResult<nLen\n", this);
		return 1;
	}
	
	return 0;
}

void CIOCPData::FlushRecv(unsigned int nLen)
{
	RecvBuffer.WriteBufferFlush(nLen);
}

int CIOCPData::Send(IPackHead *pPackHead)
{
	int nResult;

	IPacketObject* pPacketObject = g_PacketFactory.GetPacketObject( pPackHead->GetPacketDefine1());
	if( NULL==pPacketObject )
	{
		printf("Err, CIOCPData::Send:GetPacketObject,PacketDefin(%d)\n", pPackHead->GetPacketDefine1());
		return -1;
	}

	nResult = SendBuffer.WriteBufferAtom((const char*)pPackHead, pPackHead->GetPacketSize());
	if(-1==nResult)
	{
		//
		//缓冲区满
		printf("Err, CIOCPData::Send: Buffer Full!!\n");
		return 1;
	}

	return 0;
}


int CIOCPData::Recv()
{
	InitRecv();
	return RecvPacket();
}

int CIOCPData::RecvPacketHead()
{
	return RecvBuffer.TryReadBuffer(BufferPacket, name_msg_packet::PACKET_HEAD_SIZE);
}

int CIOCPData::RecvPacketContent(unsigned int nLen)
{
	return RecvBuffer.TryReadBuffer( BufferPacket, nLen+name_msg_packet::PACKET_HEAD_SIZE );
}


int CIOCPData::InitRecvPacketContent()
{
	IPackHead* pPackHead = (IPackHead*)BufferPacket;
	IPacketObject* pPacketObject = g_PacketFactory.GetPacketObject( pPackHead->GetPacketDefine1());

	if( NULL==pPacketObject )
	{
		printf("Err, CIOCPData::RecvPacketContent:GetPacketObject:%d\n", pPackHead->GetPacketDefine1());
		return -1;
	}

	return pPackHead->GetPacketContentSize();
}


void CIOCPData::InitRecv()
{
	InitRecv1();
	InitRecv2();
	nWSARecvFlag = 0;
	nEvent = IOCP_EVENT_RECV_BIG;
}

void CIOCPData::InitRecv1()
{
	int i;

	char *p = RecvBuffer.GetWriteBuffer1(&i);
	WSADataBuf[0].buf = p;
	WSADataBuf[0].len = i;

	//printf("Notic Recv len1:%d\n", i);
}

void CIOCPData::InitRecv2()
{
	int i;

	char *p = RecvBuffer.GetWriteBuffer2(&i);
	WSADataBuf[1].buf = p;
	WSADataBuf[1].len = i;

	//printf("Notic Recv len2:%d\n", i);
}

int CIOCPData::RecvPacket()
{
	int nResult;
	DWORD dwRecvBytes;

	nResult = WSARecv(nSocket, WSADataBuf, 2, &dwRecvBytes, &nWSARecvFlag, &OverlappedRecv, NULL);
	if( SOCKET_ERROR==nResult )
	{
		if( WSAGetLastError() != WSA_IO_PENDING )
		{
			/*
				两种异常
				一种是套接字本身异常，这里直接设置
				一种是投递后接收到的异常，比如关闭等等。
			*/
	//		g_NetDriver.DelIOCPRecv(this, true);
			printf("Err, CIOCPData::RecvPacket:ErrID(%d)\n", WSAGetLastError());
			return -1;
		}
	}

	return 0;
}


CBelongToList::CBelongToList()
{
	Init();
}

void CBelongToList::Init()
{
	m_bSendList=false;
	m_bRecvList=false;
	m_bIOCPPost=false;
	m_bAllSocket=false;
}

void CBelongToList::SetSendList(bool b)
{
	m_bSendList = b;
}
void CBelongToList::SetRecvList(bool b)
{
	m_bRecvList = b;
}
void CBelongToList::SetIOCPPost(bool b)
{
	m_bIOCPPost = b;
}
void CBelongToList::SetAllSocketList(bool b)
{
	m_bAllSocket = b;
}

bool CBelongToList::IsSendList()
{
	return true==m_bSendList;
}
bool CBelongToList::IsRecvList()
{
	return true==m_bRecvList;
}
bool CBelongToList::IsIOCPPost()
{
	return true==m_bIOCPPost;
}
bool CBelongToList::IsAllSocketList()
{
	return true==m_bAllSocket;
}
