#include "CNLBridgeQueue.h"
#include "MTASSERT.h"

initialiseSingleton(CNLBridgeQueue);

CNLBridgeQueue::CNLBridgeQueue()
{
	Init();
}

CNLBridgeQueue::~CNLBridgeQueue()
{
	Release();
}

void CNLBridgeQueue::Init()
{
	m_pLogicTaskQueue = new CCircleBuffer;
	m_pNetTaskQueue = new CCircleBuffer;

	m_pNetTaskQueue->Init(net_config::DEFAULT_BUFFER_LEN);
	m_pLogicTaskQueue->Init(net_config::DEFAULT_BUFFER_LEN);
}
void CNLBridgeQueue::Release()
{
//#pragma  todo 判断是否有残余的包
	delete m_pLogicTaskQueue;
	delete m_pNetTaskQueue;
}


bool CNLBridgeQueue::GetFromLogicQueue(char *pBuffer, int nBufferLen)
{
	IF(nBufferLen!=NET_PACKET_BUFF_SIZE || NULL==pBuffer)
	{
		return false;
	}

	return GetQueue(m_pLogicTaskQueue, pBuffer, nBufferLen);
}

/*
	网络层来存放
*/
bool CNLBridgeQueue::PutToLogicQueue(IPacketHead* pPacketHead)
{
	IFn(!pPacketHead)
		return false;

	char* pBuffer = (char*)(pPacketHead);
	int nBufferLen = pPacketHead->GetPacketSize();

	if (!m_pLogicTaskQueue->WriteBufferAtom(pBuffer, nBufferLen))
	{
		LOGNE("CNLBridgeQueue::PutToLogicQueue,-1==PutToLogicQueue.nDefine1:%d, nDefine2:%d\n", 
			pPacketHead->GetPacketDefine1(), pPacketHead->GetPacketDefine2());

		return false;
	}

	return true;

	//IFn(NULL==pRecvBuffer)
	//{
	//	return -1;
	//}

	//IPacketHead PackHead;
	//char BufferPacket[NET_PACKET_BUFF_SIZE]={0};

	////读头部数据大小字段
	//if( -1==pRecvBuffer->TryReadBuffer((char*)&PackHead,  NET_PACKET_HEAD_SIZE) )
	//{
	//	return 1;
	//}

	////打包投入到m_ToLogicQueue
	//if( !m_pLogicTaskQueue->WriteBufferAtom(BufferPacket, PackHead.GetPacketSize()) )
	//{
	//	return 1;
	//}
	//
	////记得刷
	//pRecvBuffer->ReadBufferFlush(PackHead.GetPacketSize());

	////长度我随便写的，体检检测下空间大小，如果不够直接投递，不必等到下次
	//if (pRecvBuffer->GetUseLength() < (NET_PACKET_HEAD_SIZE * 2)) 
	//{
	//	return 1;
	//}

	//return 0;
}


bool CNLBridgeQueue::GetFromNetQueue(char *pBuffer, int nBufferLen)
{
	IFn(!pBuffer)
	{
		return false;
	}

	return GetQueue(m_pNetTaskQueue, pBuffer, nBufferLen);
}

bool CNLBridgeQueue::PutToNetQueue(IPacketHead* pPacketHead)
{
	IFn(!pPacketHead)
		return false;

	char* pBuffer = (char*)(pPacketHead);
	int nBufferLen = pPacketHead->GetPacketSize();

	if (!m_pNetTaskQueue->WriteBufferAtom(pBuffer, nBufferLen))
	{
		LOGNE("CNLBridgeQueue::PutNetTaskQueue,-1==PutNetTaskQueue.nDefine1:%d, nDefine2:%d\n", 
			pPacketHead->GetPacketDefine1(), pPacketHead->GetPacketDefine2());

		return false;
	}
	
	return true;
}


bool CNLBridgeQueue::GetQueue(CCircleBuffer *pSrcCircleBuffer, char *pDstBuffer, int nBufferLen)
{
	IFn(!pSrcCircleBuffer || !pDstBuffer)
		return false;

	int nPacketIdent=0;

	IPacketHead PackHead;
	if (!pSrcCircleBuffer->TryReadBuffer((char*)&PackHead, NET_PACKET_HEAD_SIZE) )
	{
		return false;
	}

	IFn(PackHead.GetPacketSize()>nBufferLen)
	{
		LOGNE("CNLBridgeQueue::GetQueue. PacketID1:%d, PacketID2:%d, PacketSize:%d\n", 
			PackHead.GetPacketDefine1(), PackHead.GetPacketDefine2(), PackHead.GetPacketSize());
		return false;
	}

	return pSrcCircleBuffer->ReadBufferAtom(pDstBuffer, PackHead.GetPacketSize());
}