#include "CSocketClient.h"
#include "CWindowsSlabManager.h"
#include "CSocketAPI.h"
#include "GlobalMacro.h"

SOCKET_KEY_SEED = 0;

CSocketClient*  MallocSocketClientObject()
{
	//void *p = name_slab::kmem_malloc(sizeof(CSocketClient));	

	//IFn( NULL==p )
	//{
	//	return NULL;
	//}

	//CSocketClient *pSocketClient = new(p)CSocketClient;

	//return pSocketClient;

	CSocketClient *pSocketClient = new CSocketClient;
	
	if (pSocketClient)
	{
		pSocketClient->m_nKey = SOCKET_KEY_SEED++;
	}
	return pSocketClient;
}
void  FreeSocketClientObject(CSocketClient* p)
{
	//根据C++对象原则，主动调用析构函数
	//printf("CSocketRun::FreeNetObject, %x\n", p);
	//IFn(NULL==p)
	//{
	//	return;
	//}
	//p->~CSocketClient();

	//name_slab::kmem_free((void*)p);
	SAFE_DELETE(p);
}

CSocketClient::CSocketClient()
{
	m_nKey = 0;
	m_bAutoConnect = false;
	Init();
}

CSocketClient::~CSocketClient()
{
	closesocket(m_nSocket);
}

void CSocketClient::Init()
{
	m_nStepFlag = IOCP_STEP_INIT;
	m_nIP = m_nPort = m_nSocket = 0;

	m_RecvBuffer.Init(socket_circle_config::RECV_CIRCLE_LEN);
	m_SendBuffer.Init(socket_circle_config::SEND_CIRCLE_LEN);
	INIT_LIST_HEAD(&m_List);
}

unsigned int CSocketClient::GetKey()
{
	return m_nKey;
}

int CSocketClient::Send(const char* pBuffer, int nBufferLen)
{
	if (NULL==pBuffer)
	{
		return -1;
	}

	return m_SendBuffer.WriteBufferAtom(pBuffer, nBufferLen);
}

int CSocketClient::FlushSend()
{
	int nResult;

	/*
		@环形缓冲区，避免数据在末尾，flush两次
	*/
	nResult = FlushSendSub();
	if( -1==nResult )
	{
		return -1;
	}

	nResult = FlushSendSub();
	if( -1==nResult )
	{
		return -1;
	}

	return nResult;
}

int CSocketClient::FlushSendSub()
{
	int nResult;
	int nLen=0;
	const char *pSendBuffer;

	pSendBuffer = m_SendBuffer.GetReadBuffer(&nLen);

	nResult = CSocketAPI::SendnTCP(m_nSocket, pSendBuffer, nLen);
	if( -1==nResult )
	{
		return -1;
	}

	m_SendBuffer.ReadBufferFlush(nResult);
	if( nResult<nLen ) /*调试，发送不完全，等待下回*/
	{				
		return 1;
	}

	return 0;
}

int CSocketClient::Recv()
{
	InitRecv();
	return RecvPacketStream();
}

void CSocketClient::InitRecv()
{
	InitRecv1();
	InitRecv2();	
	m_nWSARecvFlag = 0;
	m_nStepFlag = IOCP_STEP_RECV_ING;
	m_nIOCPEvent = IOCP_EVENT_RECV_BIG;
	memset(&m_OverlappedRecv, 0, sizeof(m_OverlappedRecv));
}
void CSocketClient::InitRecv1()
{
	int i;

	char *p = m_RecvBuffer.GetWriteBuffer1(&i);
	m_WSADataBuf[0].buf = p;
	m_WSADataBuf[0].len = i;
}

void CSocketClient::InitRecv2()
{
	int i;

	char *p = m_RecvBuffer.GetWriteBuffer2(&i);
	m_WSADataBuf[1].buf = p;
	m_WSADataBuf[1].len = i;
}

int CSocketClient::RecvPacketStream()
{
	int nResult;
	DWORD dwRecvBytes;

	nResult = WSARecv(m_nSocket, m_WSADataBuf, 2, &dwRecvBytes, &m_nWSARecvFlag, &m_OverlappedRecv, NULL);
	if( SOCKET_ERROR==nResult )
	{
		IFn( WSAGetLastError() != WSA_IO_PENDING )
		{
			/*
				两种异常
				@一种是套接字本身异常，这里直接设置
				@一种是投递后接收到的异常，比如关闭等等。
			*/
			return -1;
		}
	}

	return 0;
}

int CSocketClient::Connect()
{
	IFn(false==InitConnect())
	{
		return -1;
	}

	/*
		windows对非阻塞会产生另外一个线程序(debug下)
	*/
	int nResult;
	nResult = CSocketAPI::ConnectNonb(m_nSocket, (sockaddr*)&m_ConnectAddr, sizeof(m_ConnectAddr), 0);
	IFn( -1==nResult )
	{
		return -1;
	}

	return 0;
}

bool CSocketClient::InitConnect()
{
	//m_nIOCPEvent = IOCP_EVENT_ACCEPT_CONNECT;	 这里设置没什么意义
	IFn( false==CSocketAPI::InitSocketTCPC(m_nSocket, m_nIP, m_nPort))
	{
		return false;
	}
	ZeroMemory(&m_ConnectAddr, sizeof(m_ConnectAddr));
	m_ConnectAddr.sin_family	= AF_INET;
	m_ConnectAddr.sin_port	    = htons(m_nPort);
	m_ConnectAddr.sin_addr.s_addr = m_nIP;

	return true;
}