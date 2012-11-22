#include "CSocketServer.h"
#include "GlobalMacro.h"
#include "mystdio.h"


CSocketServer::CSocketServer()
{
	Init();
}

CSocketServer::~CSocketServer()
{
	closesocket(m_nSocket);
}

void CSocketServer::Init()
{
	MEMSET(&m_szName, 0, sizeof(m_szName));	
	m_nIP = 0;
	m_nPort = 0;
	m_nSocket = 0;

	//OS API需要
	MEMSET(&m_OverlappedAccept, 0, sizeof(m_OverlappedAccept));
//	LPFN_ACCEPTEX m_lpfnAcceptEx;	外部初始化
}

bool CSocketServer::Accept(CSocketClient *pSocketClient)
{
	IFn(NULL==pSocketClient)
	{
		return false;
	}

	if( false==InitAccept(pSocketClient) )
	{
		return false;
	}


	int nResult;
	nResult= m_lpfnAcceptEx(m_nSocket,
							pSocketClient->m_nSocket,
							NULL,  //不知道给NULL会不会出错
							0,
							sizeof(sockaddr_in) + 16, 
							sizeof(sockaddr_in) + 16, 
							&pSocketClient->m_nRecvBytes, 
							&m_OverlappedAccept);

	if(0==nResult)
	{
		IFn(WSAGetLastError() != WSA_IO_PENDING)
		{
			/* 这段是摘录apache
			We can get here when:
			* 1) the client disconnects early
			* 2) TransmitFile does not properly recycle the accept socket (typically
			*    because the client disconnected)
			* 3) there is VPN or Firewall software installed with buggy AcceptEx implementation
			* 4) the webserver is using a dynamic address that has changed
			   我的补充:
			   如果主线程结束，则会返回句柄无效
			
			*/
			return false;
		}
	}

	/*
		如果改为有超时的，接收起来非常慢
	*/
	nResult = WaitForSingleObject(m_OverlappedAccept.hEvent, INFINITE);
	IFn( WAIT_FAILED==nResult )
	{
		return false;
	}


	return true;
}

bool CSocketServer::InitAccept(CSocketClient *pSocketClient)
{
	/*
		这里要注意 closesocket,否则之前超时的套接
		字再也无法返回
	*/
	pSocketClient->m_nIOCPEvent = IOCP_EVENT_ACCEPT_CONNECT;
	pSocketClient->m_nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	IFn(INVALID_SOCKET == pSocketClient->m_nSocket) 
	{
		return false;
	}

	IFn(FALSE== ResetEvent(m_OverlappedAccept.hEvent))
	{
		return false;
	}

	return true;
}
