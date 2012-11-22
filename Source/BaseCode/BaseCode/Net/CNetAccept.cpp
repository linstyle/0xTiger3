
#include "CNetAccept.h"
#include "CSocketAPI.h"
#include <process.h>

CNetAccept::CNetAccept()
{
	
}

CNetAccept::~CNetAccept()
{
	Release();
}

void CNetAccept::Init(CIOCP *pIOCP)
{
	m_hThreadAccept = NULL;
	m_uThreadAccept= 0;
	m_pIOCP = pIOCP;

	INITASSERT(!m_pIOCP);
	
	InitThread();
}

void CNetAccept::Release()
{
	ReleaseThread();
}

void CNetAccept::InitThread()
{
	m_hThreadAccept = (HANDLE)_beginthreadex(NULL, 0, ThreadAccept, this, 0, &m_uThreadAccept);	
	INITASSERT( 0== m_hThreadAccept);
}

void CNetAccept::ReleaseThread()
{
	m_bThreadRun = false;

	IFn( WAIT_FAILED==WaitForSingleObject(m_hThreadAccept, INFINITE) )
	{

	}
	CloseHandle(m_hThreadAccept);
}

bool CNetAccept::SetSocketServer(const char* pName, const char* pListenIP, USHORT nListenPort)
{	
	//服务端套接字，初始化的时候必须成功
	memcpy(m_SocketServer.m_szName, pName, sizeof(m_SocketServer.m_szName)-1);

	m_SocketServer.m_nSocket = 0;
	m_SocketServer.m_nIP = inet_addr(pListenIP);
	m_SocketServer.m_nPort = htons(nListenPort);

	m_SocketServer.m_OverlappedAccept.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	INITASSERT( !m_SocketServer.m_OverlappedAccept.hEvent );
	INITASSERT( !CSocketAPI::InitSocketTCPS(m_SocketServer.m_nSocket, m_SocketServer.m_nIP, m_SocketServer.m_nPort));
	INITASSERT( !InitAccpetExlpfn(&m_SocketServer));
	
	return true;
}

bool CNetAccept::InitAccpetExlpfn(CSocketServer *pSocketServer)
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

bool CNetAccept::IOCPPostConnect(CSocketClient *pSocketClient)
{
	IFn(!m_pIOCP)
		return false;

	IFn( m_pIOCP && FALSE==m_pIOCP->PostStatus((ULONG_PTR)pSocketClient, 1) )
	{
		return false;
	}	

	return true;
}

CSocketServer* CNetAccept::GetSocketServer()
{
	return &m_SocketServer;
}

unsigned int WINAPI CNetAccept::ThreadAccept(void* pParam)
{
	IFn(NULL==pParam)
		return 0;

	CNetAccept *pNetAccept=(CNetAccept*)pParam;
	CSocketClient *pSocketClient = NULL;
	CSocketServer* pSocketServer = pNetAccept->GetSocketServer();

	IFn(NULL==pSocketServer)
		return 0;

	while(pNetAccept->m_bThreadRun)
	{
		//pNetAccept->m_bThreadRun，防止中途退出导致卡住
		if (NULL==pSocketClient)
		{
			pSocketClient = MallocSocketClientObject();
		}

		IFn(NULL==pSocketClient)
		{
			Sleep(1000);
			continue;
		}

		if( !pSocketServer->Accept(pSocketClient) )
		{
			FreeSocketClientObject(pSocketClient);
			pSocketClient = NULL;
			continue;
		}

		//投递到完成端口
		IFn( !pNetAccept->IOCPPostConnect(pSocketClient) )
		{
			FreeSocketClientObject(pSocketClient);
			pSocketClient = NULL;
		}

		pSocketClient = NULL;
			
	}//end while

	return 0;
}