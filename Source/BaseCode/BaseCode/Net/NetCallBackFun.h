/*
	@lindp lin_style@foxmail.com   
	@2012/2/8
	@网络层的回调函数实现,比如收到网络数据后怎么处理。可以看成网络层和上层的一道粘贴膜。如果想替换网络库，
	 直接修改该代码即可。
*/
#pragma  once
#include "Singleton.h"
#include "CCircleBuffer.h"

namespace net_config
{
	//按百兆网卡计算，15MB
	const int DEFAULT_BUFFER_LEN = 1024*15*1024; 

};


class CNetBridgeQueue: public Singleton<CNetBridgeQueue>
{
public:
	CNetBridgeQueue();
	~CNetBridgeQueue();

	/*
	    操作m_ToLogicQueue
		@GetLogicQueue
		    返回值 -1:失败  0:成功 1:长度不够

		@PutLogicTaskQueue
		    返回值 -1:失败  0:成功 1:包长度不够，要继续投递  
	*/
	int GetLogicTaskQueue(char *pBuffer, int nBufferLen);
	int PutLogicTaskQueue(CCircleBuffer *pRecvBuffer);	
	int PutLogicTaskQueue(char *pBuffer, int nBufferLen);

	/*
		操作m_ToNetQueue
		@GetNetTaskQueue
            返回值 -1:失败  0:成功 1:长度不够

		@PutNetQueue:逻辑层来放
		    返回值 -1:失败  0:成功 1:长度不够
	*/
	int GetNetTaskQueue(char *pBuffer, int nBufferLen);
	int PutNetTaskQueue(const char *pBuffer, int nBufferLen);	

private:
	void Init();
	void Release();

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

#define g_NetBridgeQueue CNetBridgeQueue::getSingleton()