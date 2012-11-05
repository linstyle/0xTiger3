/*  作者：		林东平                                                              
	创建时间：	2010/07/14                                                         
    功能说明：  完成端口封装，代码参考《windows 核心编程 5th》
*/

#pragma once
#include <winsock2.h>
#include <windows.h>

#include "CmnAssert.h"


class CIOCP 
{
public:
	CIOCP() 
	{
		m_hIOCP = NULL; 
	}

	~CIOCP() 
	{ 
		if (m_hIOCP != NULL) 
			chVERIFY(CloseHandle(m_hIOCP)); 
	}


	BOOL Close() 
	{
		BOOL bResult = CloseHandle(m_hIOCP);
		m_hIOCP = NULL;
		return(bResult);
	}

	BOOL Create(int nMaxConcurrency = 0) 
	{
		m_hIOCP = CreateIoCompletionPort(
			INVALID_HANDLE_VALUE, NULL, 0, nMaxConcurrency);
		chASSERT(m_hIOCP != NULL);
		return(m_hIOCP != NULL);
	}

	BOOL AssociateDevice(HANDLE hDevice, ULONG_PTR CompKey) 
	{
		BOOL fOk = (CreateIoCompletionPort(hDevice, m_hIOCP, CompKey, 0) 
			== m_hIOCP);
//		chASSERT(fOk);
		return(fOk);
	}

	BOOL AssociateSocket(SOCKET hSocket, ULONG_PTR CompKey) 
	{
		return(AssociateDevice((HANDLE) hSocket, CompKey));
	}

	BOOL PostStatus(ULONG_PTR CompKey, DWORD dwNumBytes = 0, 
		OVERLAPPED* po = NULL) 
	{

			BOOL fOk = PostQueuedCompletionStatus(m_hIOCP, dwNumBytes, CompKey, po);
			chASSERT(fOk);
			return(fOk);
	}

	BOOL GetStatus(ULONG_PTR* pCompKey, PDWORD pdwNumBytes,
		OVERLAPPED** ppo, DWORD dwMilliseconds = INFINITE) 
	{

			return(GetQueuedCompletionStatus(m_hIOCP, pdwNumBytes, 
				pCompKey, ppo, dwMilliseconds));
	}

private:
	HANDLE m_hIOCP;
};