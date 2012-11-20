/*
	@lindp lin_style@foxmail.com   
	@2012/2/8
	@�������߼���֮���ͨѶ����
*/
#pragma  once
#include "Singleton.h"
#include "CCircleBuffer.h"
#include "PPackets.h"

namespace net_config
{
	//�ݶ�15MB.����ʵ�ʲ���ʱ��ÿ��������*�������ӳ�����
	const int DEFAULT_BUFFER_LEN = 1024*15*1024; 
};


class CNLBridgeQueue: public Singleton<CNLBridgeQueue>
{
public:
	/*
		����GET,SET����֤ȡ�ػ�д��һ�������İ�.
	*/
	CNLBridgeQueue();
	~CNLBridgeQueue();

	bool GetFromLogicQueue(char *pBuffer, int nBufferLen);
	bool PutToLogicQueue(CCircleBuffer *pRecvBuffer);	

	bool GetFromNetQueue(char *pBuffer, int nBufferLen);
	bool PutToNetQueue(IPacketHead* pPacketHead);

private:
	void Init();
	void Release();

	bool GetQueue(CCircleBuffer *pSrcCircleBuffer, char *pDstBuffer, int nBufferLen);

public:

private:
	/*
		@m_pLogicTaskQueue
		�߼�������У����������룬�߼���ȡ��
		@m_pNetTaskQueue
		����������У����߼�����룬�����ȡ��
	*/
	CCircleBuffer *m_pLogicTaskQueue;
	CCircleBuffer *m_pNetTaskQueue;
};

#define g_NLBridgeQueue CNLBridgeQueue::getSingleton()