#include "CNLBridgeQueue.h"


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
//#pragma  todo �ж��Ƿ��в���İ�
	delete m_pLogicTaskQueue;
	delete m_pNetTaskQueue;
}


int CNLBridgeQueue::GetLogicTaskQueue(char *pBuffer, int nBufferLen)
{
	IF(nBufferLen!=NET_PACKET_BUFF_SIZE || NULL==pBuffer)
	{
		return -1;
	}

	return GetQueue(m_pLogicTaskQueue, pBuffer, nBufferLen);
}

/*
	����������
*/
int CNLBridgeQueue::PutLogicTaskQueue(CCircleBuffer *pRecvBuffer)
{
	IFn(NULL==pRecvBuffer)
	{
		return -1;
	}

	IPacketHead PackHead;
	char BufferPacket[NET_PACKET_BUFF_SIZE]={0};

	//��ͷ�����ݴ�С�ֶ�
	if( -1==pRecvBuffer->TryReadBuffer((char*)&PackHead,  NET_PACKET_HEAD_SIZE) )
	{
		return 1;
	}

	//���Ͷ�뵽m_ToLogicQueue
	if( -1==m_pLogicTaskQueue->WriteBufferAtom(BufferPacket, PackHead.GetPacketSize()) )
	{
		return 1;
	}
	
	//�ǵ�ˢ
	pRecvBuffer->ReadBufferFlush(PackHead.GetPacketSize());

	//���������д�ģ�������¿ռ��С���������ֱ��Ͷ�ݣ����صȵ��´�
	if (pRecvBuffer->GetUseLength() < (NET_PACKET_HEAD_SIZE * 2)) 
	{
		return 1;
	}

	return 0;
}

int CNLBridgeQueue::PutLogicTaskQueue(char *pBuffer, int nBufferLen)
{
	IFn(NULL==pBuffer)
	{
		return -1;
	}

	return m_pLogicTaskQueue->WriteBufferAtom(pBuffer, nBufferLen);
}

int CNLBridgeQueue::GetNetTaskQueue(char *pBuffer, int nBufferLen)
{
	IFn(nBufferLen!=NET_PACKET_BUFF_SIZE || NULL==pBuffer)
	{
		return -1;
	}

	return GetQueue(m_pNetTaskQueue, pBuffer, nBufferLen);
}

int CNLBridgeQueue::PutNetTaskQueue(IPacketHead* pPacketHead)
{
	char* pBuffer = (char*)(pPacketHead);
	int nBufferLen = pPacketHead->GetPacketSize();

	int nResult = PutNetTaskQueue(pBuffer, nBufferLen);
	if(-1==nResult)
	{
		LOGNE("CNLBridgeQueue::PutNetTaskQueue,-1==PutNetTaskQueue.nDefine1:%d, nDefine2:%d\n", 
			pPacketHead->GetPacketDefine1(), pPacketHead->GetPacketDefine2());
	}

	return nResult;
}

int CNLBridgeQueue::PutNetTaskQueue(const char *pBuffer, int nBufferLen)
{
	IF(NULL==pBuffer)
	{
		return -1;
	}

	return m_pNetTaskQueue->WriteBufferAtom(pBuffer, nBufferLen);
}

int CNLBridgeQueue::GetQueue(CCircleBuffer *pSrcCircleBuffer, char *pDstBuffer, int nBufferLen)
{
	int nPacketIdent=0;

	IPacketHead PackHead;
	if (-1==pSrcCircleBuffer->TryReadBuffer((char*)&PackHead, NET_PACKET_HEAD_SIZE) )
	{
		return 1;
	}

	IFn(PackHead.GetPacketSize()>nBufferLen)
	{
		LOGNE("CNLBridgeQueue::GetQueue. PacketID1:%d, PacketID2:%d, PacketSize:%d\n", 
			PackHead.GetPacketDefine1(), PackHead.GetPacketDefine2(), PackHead.GetPacketSize());
		return 1;
	}

	return pSrcCircleBuffer->ReadBufferAtom(pDstBuffer, PackHead.GetPacketSize());
}