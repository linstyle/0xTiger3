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