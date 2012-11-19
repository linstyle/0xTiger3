/*
	@lindp lin_style@foxmail.com   
	@2012/2/8
	@网络层和逻辑层之间的通讯队列
*/
#pragma  once
#include "Singleton.h"
#include "CCircleBuffer.h"
#include "PPackets.h"

namespace net_config
{
	//按百兆网卡计算，15MB
	const int DEFAULT_BUFFER_LEN = 1024*15*1024; 

};


class CNLBridgeQueue: public Singleton<CNLBridgeQueue>
{
public:
	/*
		统一返回值
		-1:失败  0:成功 1:长度不够
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
		逻辑任务队列，由网络层放入，逻辑层取出
		@m_pNetTaskQueue
		网络任务队列，由逻辑层放入，网络层取出
	*/
	CCircleBuffer *m_pLogicTaskQueue;
	CCircleBuffer *m_pNetTaskQueue;
};

#define g_NLBridgeQueue CNLBridgeQueue::getSingleton()