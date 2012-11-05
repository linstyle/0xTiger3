#include "StdAfx.h"
#include "CSocketRun.h"
#include <process.h>


unsigned int WINAPI CSocketRun::ProcAccept(void* param)
{
	name_socket_run::THREAD_PARAM_ACCPET *pTAcceptParam = 
		(name_socket_run::THREAD_PARAM_ACCPET *)param;
	CSocketRun *pSocketRun = pTAcceptParam->pSocketRun;
	ILocalObjectAccept *pObjectAccept = pTAcceptParam->pObjectAccept;

	int nResult;
	CIOCPData *p = NULL;
	while(true==pSocketRun->IsThreadRun())
	{
		p = pSocketRun->MallocNetObject();
		printf("Notic, CSocketRun::ProcAccept:Malloc %x:\n", p);
		if( NULL==p )
		{
			Sleep(1000);
			continue;
		}		

		nResult = pObjectAccept->Accept(p);
		if( -1==nResult || false==pSocketRun->IsThreadRun())
		{			
			//debug,³ö´í¼ÆÊý.
			++g_NetTracer.m_nAcceptErr;
			pSocketRun->FreeNetObject(p);			
			continue;
		}

		pSocketRun->IOCPPostConnect(p);
	}

	delete param;
	printf("Notic, CSocketRun::ProcAccept:Free %x:\n", p);
	return 0;
}

bool CSocketRun::StartThreadAccept()
{
	unsigned int tid;	
	HANDLE handle;
	name_socket_run::THREAD_PARAM_ACCPET *pTAcceptParam;

	for(unsigned int i=0; i<m_LocalObjectAccept.size(); ++i)
	{
		pTAcceptParam = new name_socket_run::THREAD_PARAM_ACCPET;
		pTAcceptParam->pObjectAccept = m_LocalObjectAccept[i];
		pTAcceptParam->pSocketRun = this;

		handle = (HANDLE)_beginthreadex(NULL, 0, ProcAccept,(void *)pTAcceptParam, 0, &tid);		
		INITASSERT( 0==handle );
		m_hThreadAccept.push_back(handle);
	}
	return true;
}
void CSocketRun::EndThreadAccept()
{
	int nCount = m_LocalObjectAccept.size();
	for(int i=0; i<nCount; ++i)
	{
		if( WAIT_FAILED==WaitForSingleObject(m_hThreadAccept[i], INFINITE) )
		{
			printf("Err, CSocketRun::EndThreadAccept:ErrID(%d)\n", WSAGetLastError());
			CloseHandle(m_hThreadAccept[i]);
		}
	}
}

void CSocketRun::AddLocalAccept(ILocalObjectAccept* pLocalObject)
{
	m_LocalObjectAccept.push_back(pLocalObject);
}

