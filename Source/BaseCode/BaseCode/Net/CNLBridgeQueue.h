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
	//暂定15MB.根据实际测试时，每秒吞吐量*可容忍延迟秒数
	const int DEFAULT_BUFFER_LEN = 1024*15*1024; 
};


class CNLBridgeQueue: public Singleton<CNLBridgeQueue>
{
public:
	/*
		无论GET,SET都保证取回或写入一个完整的包.
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
		逻辑任务队列，由网络层放入，逻辑层取出
		@m_pNetTaskQueue
		网络任务队列，由逻辑层放入，网络层取出
	*/
	CCircleBuffer *m_pLogicTaskQueue;
	CCircleBuffer *m_pNetTaskQueue;
};

#define g_NLBridgeQueue CNLBridgeQueue::getSingleton()