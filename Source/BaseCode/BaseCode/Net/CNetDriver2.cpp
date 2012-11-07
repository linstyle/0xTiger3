#include "CNetDriver2.h"
#include "GlobalMacro.h"
#include <process.h>
initialiseSingleton(CNetDriver2);


CNetDriver2::CNetDriver2()
{
	//公共队列
	new CNetBridgeQueue;

	m_bHasInit = false;
	InitSocketLib();	
}

CNetDriver2::~CNetDriver2()
{
	Release();
}

bool CNetDriver2::SetSocketServer(const char* pName, const char* pListenIP, USHORT nListenPort)
{
	IFn(!pName && pListenIP)
		return false;

	INITASSERT(!m_NetAcceptThread.SetSocketServer(pName, pListenIP, nListenPort));
	
	m_bHasInit = true;
	return true;
}

bool CNetDriver2::AddClientSocket(const char* pConnectIP, USHORT nConnectPort)
{
	IFn(!pConnectIP)
		return false;

	PNLInnerNotic msgInnerNotic;
	msgInnerNotic.SetPacketDefine2( PACKET2_LTON_CONNECT_SOCKET);
	msgInnerNotic.m_nIP = inet_addr(pConnectIP);
	msgInnerNotic.m_nPort = nConnectPort;

	IFn (-1==g_NetBridgeQueue.PutNetTaskQueue((char*)&msgInnerNotic, msgInnerNotic.GetPacketSize()))
	{
		return false;
	}

	m_bHasInit = true;

	return true;
}

bool CNetDriver2::NoticNetErr(void *pNetObject)
{
	IFn(!pNetObject)
		return false;

	PNLInnerNotic msgInnerNotic;
	msgInnerNotic.SetPacketDefine2( PACKET2_LTON_ERR);
	msgInnerNotic.SetNetObject(pNetObject);

	IFn (-1==g_NetBridgeQueue.PutNetTaskQueue((char*)&msgInnerNotic, msgInnerNotic.GetPacketSize()))
	{
		return false;
	}

	return true;
}

int CNetDriver2::GetPacketStream(char *pBuffer, int nBufferLen)
{
	IFn(!pBuffer)
		return -1;

	__try
	{
		return g_NetBridgeQueue.GetLogicTaskQueue(pBuffer, nBufferLen);
	}
	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))
	{
		return false;
	}	
	
}

int CNetDriver2::PutPacketStream(const char *pBuffer, int nBufferLen)
{
	IFn(!pBuffer)
		return -1;

	__try
	{
		return g_NetBridgeQueue.PutNetTaskQueue(pBuffer, nBufferLen);
	}
	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))
	{
		return false;
	}	
}

void CNetDriver2::Init()
{	
	INITASSERT(!m_bHasInit);

	m_NetKernelThread.Init();
	m_NetAcceptThread.Init(m_NetKernelThread.GetIOCP());
}

void CNetDriver2::Release()
{
	ReleaseAllConnect();
	ReleaseSocketLib();

	delete g_NetBridgeQueue.getSingletonPtr();
}


void CNetDriver2::InitSocketLib()
{
	WSADATA WsaData;

	INITASSERT( 0!=WSAStartup(MAKEWORD(2,2),&WsaData) );
}

void CNetDriver2::ReleaseSocketLib()
{
	WSACleanup();
}



void CNetDriver2::ReleaseAllConnect()
{

}


