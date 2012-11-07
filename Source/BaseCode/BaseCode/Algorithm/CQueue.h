/*  作者：		林东平                                                              
	创建时间：	2011/1/31                                                      
    功能说明：  队列。由两个链表头组成。
				支持多线程
				Put(内容)
				const Get(指针)

				(k->buffer)->queue, (get do logic)
				(get do put) queue->(buffer->k)
*/

#pragma once

#include <stdio.h>
#include "CLock.h"
#include "LinuxList.h"

namespace name_queue
{
	const int QUEUE_COUNT = 1000;
	/*
		每个任务包的大小，单位为字节 
	*/
	const int QUEUE_NODE_SIZE = 2048;
};


struct QUEUE_NODE
{
	QUEUE_NODE()
	{
		pBuffer = NULL;
		INIT_LIST_HEAD(&ListNext);
	}
	~QUEUE_NODE()
	{
		delete []pBuffer;
	}

	void Init(int nBufferSize)
	{
		pBuffer = new char[nBufferSize];
		nSize = nBufferSize;
	}

	char *pBuffer;
	int nSize;

	struct list_head ListNext;
};

class CQueue
{
public:
	~CQueue();	

	void Init(int nQueueNodeSize=name_queue::QUEUE_NODE_SIZE, int nQueueCount=name_queue::QUEUE_COUNT);
	void Tracer();

	/*
		以下是两套接口:
		传统的内容拷贝
		GetUseNode：取一个有效的任务节点,并把该节点放到空闲队列中
		PutUseNode:释放一个任务节点到有效的链表中
		
		在一些需要效率的地方，直接暴露指针给外部,并且需要手动归还指针接点
		GetUseNodePtr:取一个有效的任务节点指针
		GetFreeNodePtr:取一个空闲的任务节点指针
		PutUseNodePtr:释放一个任务节点到有效的链表中
		PutFreeNodePtr:释放一个任务节点到空闲的链表中
	*/
	int GetUseNode(char* pBuffer, int nSize);
	bool PutUseNode(const char* pBuffer, int nSize);

	QUEUE_NODE* GetUseNodePtr();
	QUEUE_NODE* GetFreeNodePtr();
	void PutUseNodePtr(QUEUE_NODE* pQueueNode);
	void PutFreeNodePtr(QUEUE_NODE* pQueueNode);

private:
	
	void InitList();

	QUEUE_NODE* _GetUseNode();
	QUEUE_NODE* _GetFreeNode();

	void _PutUseNode(QUEUE_NODE* p);
	void _PutFreeNode(QUEUE_NODE* p);

public:

private:
	int m_nQueueNodeSize;
	int m_nQueueNodeCount;
	QUEUE_NODE *m_pQueueNode;
	list_head m_ListUse;
	list_head m_ListFree;

	CAtom m_nCountListUse;
	CAtom m_nCountListFree;

	CFastLock m_LockListUse;
	CFastLock m_LockListFree;
};

	//CQueue Queue(10, 20);

	//Queue.TracerMe();

	///*
	//	测试个数是否对
	//*/
	//int i;
	//for(i=0; i<10; ++i)
	//{
	//	char buffer[8];
	//	memset(buffer, 0, 8);
	//	sprintf(buffer, "%d", i);
	//	Queue.PutQueueNode(buffer, strlen(buffer));
	//}
	////Queue.TracerMe();

	//for(i=10; i<25; ++i)
	//{
	//	char buffer[8];
	//	memset(buffer, 0, 8);
	//	sprintf(buffer, "%d", i);
	//	Queue.PutQueueNode(buffer, strlen(buffer));
	//}
	////Queue.TracerMe();

	//for(i=0; i<10; ++i)
	//{
	//	int nSize=8;
	//	char buffer[8];
	//	memset(buffer, 0, 8);
	//	Queue.GetQueueUseNode(buffer, &nSize);
	//}

	//for(i=100; i<105; ++i)
	//{
	//	char buffer[8];
	//	memset(buffer, 0, 8);
	//	sprintf(buffer, "%d", i);
	//	Queue.PutQueueNode(buffer, strlen(buffer));
	//}
	//Queue.TracerMe();
