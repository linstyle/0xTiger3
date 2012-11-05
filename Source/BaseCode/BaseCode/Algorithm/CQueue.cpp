#include "StdAfx.h"
#include "CQueue.h"

CQueue::~CQueue()
{
	delete []m_pQueueNode;
}

void CQueue::Init(int nQueueNodeSize, int nQueueCount)
{
	m_nQueueNodeSize = nQueueNodeSize;
	m_nQueueNodeCount = nQueueCount;

	InitList();
}

void CQueue::InitList()
{
	m_nCountListUse = 0;
	m_nCountListFree = m_nQueueNodeCount;

	INIT_LIST_HEAD(&m_ListUse);
	INIT_LIST_HEAD(&m_ListFree);

	/*
		空间初始化
	*/
	m_pQueueNode = new QUEUE_NODE[m_nQueueNodeCount];
	int i;
	for(i=0; i<m_nQueueNodeCount; ++i)
	{
		m_pQueueNode[i].Init(m_nQueueNodeSize);
	}

	/*
		形成链表
	*/
	for(i=0; i<m_nQueueNodeCount; ++i)
	{
		list_add(&m_pQueueNode[i].ListNext, &m_ListFree);
	}
}

int CQueue::GetUseNode(char* pBuffer, int nSize)
{
	if( nSize>m_nQueueNodeSize )
	{
		return -1;
	}

	QUEUE_NODE* pUseQueueNode;
	/*
		获取一个Use的节点
	*/
	pUseQueueNode = _GetUseNode();
	if(NULL==pUseQueueNode)
	{
		return -1;
	}

	memcpy((void*)pBuffer, pUseQueueNode->pBuffer, pUseQueueNode->nSize);

	/*
		使该节点空闲
	*/
	_PutFreeNode(pUseQueueNode);

	return pUseQueueNode->nSize;
}

bool CQueue::PutUseNode(const char* pBuffer, int nSize)
{
	if( nSize>m_nQueueNodeSize )
	{
		return false;
	}

	QUEUE_NODE* pFreeQueueNode;

	/*
		获取一个空闲节点
	*/
	pFreeQueueNode = _GetFreeNode();
	if(NULL==pFreeQueueNode)
	{
		return false;
	}

	memcpy( (void*)pFreeQueueNode->pBuffer, pBuffer, nSize);
	pFreeQueueNode->nSize = nSize;

	/*
		使该节点有效
	*/
	_PutUseNode(pFreeQueueNode);

	return true;
}


QUEUE_NODE* CQueue::GetUseNodePtr()
{
	return _GetUseNode();
}

QUEUE_NODE* CQueue::GetFreeNodePtr()
{
	return _GetFreeNode();
}

void CQueue::PutUseNodePtr(QUEUE_NODE* pQueueNode)
{
	_PutUseNode(pQueueNode);
}
void CQueue::PutFreeNodePtr(QUEUE_NODE* pQueueNode)
{
	_PutFreeNode(pQueueNode);
}

QUEUE_NODE* CQueue::_GetUseNode()
{
	list_head *pListIte;
	QUEUE_NODE* pQueueNode;

	m_LockListUse.Lock();
	pListIte = list_get(&m_ListUse);
	m_LockListUse.UnLock();	

	if( NULL==pListIte )
	{
		return NULL;
	}

	pQueueNode = list_entry_offset(pListIte, QUEUE_NODE, ListNext);
	
	--m_nCountListUse;
	return pQueueNode;
}

QUEUE_NODE* CQueue::_GetFreeNode()
{
	list_head *pListIte;
	QUEUE_NODE* pQueueNode;

	m_LockListFree.Lock();
	pListIte = list_get(&m_ListFree);
	m_LockListFree.UnLock();

	if( NULL==pListIte )
	{
		return NULL;
	}

	pQueueNode = list_entry_offset(pListIte, QUEUE_NODE, ListNext);
	
	--m_nCountListFree;
	return pQueueNode;
}

void CQueue::_PutUseNode(QUEUE_NODE* p)
{
	m_LockListUse.Lock();

	list_add_tail(&p->ListNext, &m_ListUse);
	++m_nCountListUse;

	m_LockListUse.UnLock();
}

void CQueue::_PutFreeNode(QUEUE_NODE* p)
{
	m_LockListFree.Lock();

	list_add(&p->ListNext, &m_ListFree);
	++m_nCountListFree;

	m_LockListFree.UnLock();	
}

void CQueue::Tracer()
{
	int i;
	list_head *pListIte;
	QUEUE_NODE *pQueueNode;

	printf("    CQueue:\n");
	printf("Use:%d, Free:%d\n", m_nCountListUse.GetValue(), m_nCountListFree.GetValue());
	//printf("Use List Value:\n");
	//i = 0;
	//list_for_each(pListIte, &m_ListUse)
	//{
	//	pQueueNode = list_entry_offset(pListIte, QUEUE_NODE, ListNext);
	//	printf("%d, %s\n", i, pQueueNode->pBuffer);
	//	++i;
	//}
	//
	//i = 0;
	//printf("Free List Value:\n");
	//list_for_each(pListIte, &m_ListFree)
	//{
	//	pQueueNode = list_entry_offset(pListIte, QUEUE_NODE, ListNext);
	//	printf("%d, %s\n", i, pQueueNode->pBuffer);
	//	++i;
	//}
	
}