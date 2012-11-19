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
	//�������������㣬15MB
	const int DEFAULT_BUFFER_LEN = 1024*15*1024; 

};


class CNLBridgeQueue: public Singleton<CNLBridgeQueue>
{
public:
	/*
		ͳһ����ֵ
		-1:ʧ��  0:�ɹ� 1:���Ȳ���
	*/
	CNLBridgeQueue();
	~CNLBridgeQueue();

	int GetLogicTaskQueue(char *pBuffer, int nBufferLen);
	int PutLogicTaskQueue(CCircleBuffer *pRecvBuffer);	
	int PutLogicTaskQueue(char *pBuffer, int nBufferLen);

	int GetNetTaskQueue(char *pBuffer, int nBufferLen);
	int PutNetTaskQueue(IPacketHead* pPacketHead);

private:
	void Init();
	void Release();

	int PutNetTaskQueue(const char *pBuffer, int nBufferLen);	
	int GetQueue(CCircleBuffer *pSrcCircleBuffer, char *pDstBuffer, int nBufferLen);

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