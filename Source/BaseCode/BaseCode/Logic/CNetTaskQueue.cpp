#include "StdAfx.h"
#include "CNetTaskQueue.h"

CNetTaskQueue::CNetTaskQueue()
{
	m_pNetTaskQueue = new CQueue;
	m_pNetTaskQueue->Init();
}

CNetTaskQueue::~CNetTaskQueue()
{
	delete m_pNetTaskQueue;
}

void CNetTaskQueue::TracerMe()
{
	DELAY_CONTINUE(1000*1)
	Tracer();
}

void CNetTaskQueue::Tracer()
{
	printf("Notic: CNetTaskQueue\n");
	m_pNetTaskQueue->Tracer();
}

QUEUE_NODE* CNetTaskQueue::GetUseNodePtr()
{
	return m_pNetTaskQueue->GetUseNodePtr();
}

QUEUE_NODE* CNetTaskQueue::GetFreeNodePtr()
{
	return m_pNetTaskQueue->GetFreeNodePtr();
}

void CNetTaskQueue::PutUseNodePtr(QUEUE_NODE* pQueueNode)
{
	m_pNetTaskQueue->PutUseNodePtr(pQueueNode);
}
void CNetTaskQueue::PutFreeNodePtr(QUEUE_NODE* pQueueNode)
{
	m_pNetTaskQueue->PutFreeNodePtr(pQueueNode);
}
