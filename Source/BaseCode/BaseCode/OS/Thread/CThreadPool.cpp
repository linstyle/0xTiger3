#include "StdAfx.h"
#include "CThreadPool.h"
#include <process.h>
#include <assert.h>


initialiseSingleton(CThreadPool);

THREAD_POOL_OBJECT::THREAD_POOL_OBJECT()
{
	memset(m_ThreadName, 0, sizeof(m_ThreadName));
}


static unsigned int WINAPI ThreadProc(void* param)
{
	int nResult;
	THREAD_POOL_OBJECT *pTC = (THREAD_POOL_OBJECT *)param;

	try
	{
		nResult = pTC->pThread->Run();
	}
	catch(...)
	{
		printf("%s:ThreadProc try oop \n", pTC->m_ThreadName);
	}

	return 0;
}

CThreadPool::~CThreadPool()
{
	Exit();
}

void CThreadPool::Tracer()
{
	printf("/***ThreadPool***/\n");
	printf("Thread Count:%d\n", m_ThreadSet.size());

	ThreadSet_t::iterator itr;
	for(itr=m_ThreadSet.begin(); itr!=m_ThreadSet.end(); itr++)
	{
		printf("Thread name:%s	", (*itr)->m_ThreadName);
		printf("Thread id:%d	", (*itr)->pThread->m_uThread);
		printf("\n");
	}

	printf("\n");
}

void CThreadPool::ExecuteTask(CThread* ExecutionTarget, const char* ThreadName, int nThreadNumber)
{
	assert(nThreadNumber>0);
	for(int i=0; i<nThreadNumber; ++i)
	{
		THREAD_POOL_OBJECT* pTC = StartThread(ExecutionTarget, ThreadName);		
	}
}

THREAD_POOL_OBJECT* CThreadPool::StartThread(CThread* ExecutionTarget, const char* ThreadName)
{
	THREAD_POOL_OBJECT *pTC= new THREAD_POOL_OBJECT;

	pTC->pThread = ExecutionTarget;
	strncpy(pTC->m_ThreadName, ThreadName, 16);
	pTC->m_dwStartTime = CTime::GetMilli();

	pTC->pThread->m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, pTC, 0, &pTC->pThread->m_uThread);	
	INITASSERT( 0== pTC->pThread->m_hThread);

	m_ThreadSet.push_back(pTC);

	return pTC;
}


int CThreadPool::Exit()
{
	ThreadSet_t::iterator itr;

	for(itr=m_ThreadSet.begin(); itr!=m_ThreadSet.end(); itr++)
	{
		(*itr)->pThread->Exit();
		(*itr)->pThread->Join();

		printf("Thread %s Quit, Spend Time:%d(ms)\n", (*itr)->m_ThreadName, CTime::GetMilli()-(*itr)->m_dwStartTime);
		delete *itr;
	}

	return 0;
}

