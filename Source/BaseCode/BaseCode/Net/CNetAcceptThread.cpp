
#include "CNetAcceptThread.h"
#include "CSocketAPI.h"
#include <process.h>

CNetAcceptThread::CNetAcceptThread()
{
	
}

CNetAcceptThread::~CNetAcceptThread()
{
	Release();
}

void CNetAcceptThread::Init(CIOCP *pIOCP)
{
	m_hThreadAccept = NULL;
	m_uThreadAccept= 0;
	m_nSocketServerCount = 0;
	m_pIOCP = pIOCP;
	
	InitThread();
}

void CNetAcceptThread::Release()
{
	ReleaseThread();
}

void CNetAcceptThread::InitThread()
{
	m_hThreadAccept = (HANDLE)_beginthreadex(NULL, 0, ThreadAccept, this, 0, &m_uThreadAccept);	
	INITASSERT( 0== m_hThreadAccept);
}

void CNetAcceptThread::ReleaseThread()
{
	m_bThreadRun = false;

	IFn( WAIT_FAILED==WaitForSingleObject(m_hThreadAccept, INFINITE) )
	{

	}
	CloseHandle(m_hThreadAccept);
}

bool CNetAcceptThread::AddSocketServer(const char* pName, const char* pListenIP, USHORT nListenPort)
{
	if(m_nSocketServerCount>=SOCKET_SERVER_COUNT)
	{
		LOGNE("CNetAcceptThread::AddSocketServer. m_nSocketServerCount(%d)>=SOCKET_SERVER_COUNT(%d)\n", m_nSocketServerCount, SOCKET_SERVER_COUNT);
		return false;
	}

	CSocketServer *pSocketServer = &m_SocketServer[m_nSocketServerCount];

	//init
	pSocketServer->Init();
	
	memcpy(pSocketServer->m_szName, pName, sizeof(pSocketServer->m_szName)-4);

	pSocketServer->m_nSocket = 0;
	pSocketServer->m_nIP = inet_addr(pListenIP);
	pSocketServer->m_nPort = htons(nListenPort);

	pSocketServer->m_OverlappedAccept.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	IFn( NULL==pSocketServer->m_OverlappedAccept.hEvent )
	{
		return false;
	}

	if (false==InitSocketTCPServer(pSocketServer))
	{
		CloseHandle(pSocketServer->m_OverlappedAccept.hEvent);
		return false;
	}

	if (false==InitAccpetExlpfn(pSocketServer))
	{		
		closesocket(pSocketServer->m_nSocket);
		CloseHandle(pSocketServer->m_OverlappedAccept.hEvent);
		return false;
	}
	
	m_nSocketServerCount++;
	return true;
}

bool CNetAcceptThread::InitSocketTCPServer(CSocketServer *pSocketServer)
{
	IFn(false==CSocketAPI::InitSocketTCPS(pSocketServer->m_nSocket, pSocketServer->m_nIP, pSocketServer->m_nPort))
	{
		return false;
	}

	return true;
}

bool CNetAcceptThread::InitAccpetExlpfn(CSocketServer *pSocketServer)
{
	DWORD   bytes;
	GUID GuidAcceptEx = WSAID_ACCEPTEX;

	IFn( SOCKET_ERROR==WSAIoctl(pSocketServer->m_nSocket, 
		SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidAcceptEx, 
		sizeof(GuidAcceptEx),
		&pSocketServer->m_lpfnAcceptEx, 
		sizeof(pSocketServer->m_lpfnAcceptEx), 
		&bytes, 
		NULL, 
		NULL) )
	{
		return false;
	}

	return true;
}

bool CNetAcceptThread::IOCPPostConnect(CSocketClient *pSocketClient)
{
	IFn( FALSE==m_pIOCP->PostStatus((ULONG_PTR)pSocketClient, 1) )
	{
		return false;
	}	

	return true;
}

CSocketServer* CNetAcceptThread::GetSocketServerByIndex(int nIndex)
{
	IFn(nIndex>=m_nSocketServerCount)
	{
		return NULL;
	}

	return &m_SocketServer[nIndex];
}

unsigned int WINAPI CNetAcceptThread::ThreadAccept(void* pParam)
{
	CNetAcceptThread *pNetAccept=(CNetAcceptThread*)pParam;

	CSocketClient *pSocketClient = NULL;
	while(pNetAccept->m_bThreadRun)
	{
		//pNetAccept->m_bThreadRun，防止中途退出导致卡住
		for (int i=0; i<pNetAccept->m_nSocketServerCount && pNetAccept->m_bThreadRun; ++i)
		{			
			if (NULL==pSocketClient)
			{
				pSocketClient = MallocSocketClientObject();
			}

			IFn(NULL==pSocketClient)
			{
				Sleep(1500);
				continue;
			}

			CSocketServer* pSocketServer = pNetAccept->GetSocketServerByIndex(i);
			if (NULL==pSocketServer)
			{
				continue;
			}

			if( false==pSocketServer->Accept(pSocketClient))
			{
				FreeSocketClientObject(pSocketClient);
				pSocketClient = NULL;
				continue;
			}
			
			//投递到完成端口
			if ( pNetAccept->IOCPPostConnect(pSocketClient) )
			{
				FreeSocketClientObject(pSocketClient);
				pSocketClient = NULL;
			}
			
		}//end for
	}//end while

	return 0;
}