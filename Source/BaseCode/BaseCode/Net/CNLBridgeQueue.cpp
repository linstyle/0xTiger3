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


int CNLBridgeQueue::GetFromLogicQueue(char *pBuffer, int nBufferLen)
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
int CNLBridgeQueue::PutToLogicQueue(CCircleBuffer *pRecvBuffer)
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
	if( !m_pLogicTaskQueue->WriteBufferAtom(BufferPacket, PackHead.GetPacketSize()) )
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


bool CNLBridgeQueue::GetFromNetQueue(char *pBuffer, int nBufferLen)
{
	IFn(nBufferLen!=NET_PACKET_BUFF_SIZE || NULL==pBuffer)
	{
		return false;
	}

	return GetQueue(m_pNetTaskQueue, pBuffer, nBufferLen);
}

bool CNLBridgeQueue::PutToNetQueue(IPacketHead* pPacketHead)
{
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