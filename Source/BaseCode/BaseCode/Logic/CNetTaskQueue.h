/*  作者：		林东平                                                              
	创建时间：	2011/1/24                                                      
    功能说明：  
*/

#include "CQueue.h"
#include "CommonStruct.h"

class CNetTaskQueue
{
public:
	CNetTaskQueue();
	~CNetTaskQueue();

	void TracerMe();
	void Tracer();

	QUEUE_NODE* GetUseNodePtr();
	QUEUE_NODE* GetFreeNodePtr();
	void PutUseNodePtr(QUEUE_NODE* pQueueNode);
	void PutFreeNodePtr(QUEUE_NODE* pQueueNode);

private:

public:

private:
	CQueue *m_pNetTaskQueue;
};