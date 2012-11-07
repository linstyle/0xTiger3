#include "CNetDriver2.h"
#include <process.h>
initialiseSingleton(CNetDriver2);


CNetDriver2::CNetDriver2()
{
	Init();
}

CNetDriver2::~CNetDriver2()
{
	Release();
}

bool CNetDriver2::AddSocketServer(const char* pName, const char* pListenIP, USHORT nListenPort)
{
	return m_NetAcceptThread.AddSocketServer(pName, pListenIP, nListenPort);
}

bool CNetDriver2::AddClientSocket(const char* pConnectIP, USHORT nConnectPort)
{
	PNLInnerNotic msgInnerNotic;
	msgInnerNotic.SetPacketDefine2( PACKET2_LTON_CONNECT_SOCKET);
	msgInnerNotic.m_nIP = inet_addr(pConnectIP);
	msgInnerNotic.m_nPort = nConnectPort;

	if (-1==g_NetBridgeQueue.PutNetTaskQueue((char*)&msgInnerNotic, msgInnerNotic.GetPacketSize()))
	{
		return false;
	}

	return true;
}

bool CNetDriver2::NoticNetErr(void *pNetObject)
{
	PNLInnerNotic msgInnerNotic;
	msgInnerNotic.SetPacketDefine2( PACKET2_LTON_ERR);
	msgInnerNotic.SetNetObject(pNetObject);

	if (-1==g_NetBridgeQueue.PutNetTaskQueue((char*)&msgInnerNotic, msgInnerNotic.GetPacketSize()))
	{
		return false;
	}

	return true;
}

int CNetDriver2::GetPacketStream(char *pBuffer, int nBufferLen)
{
	return g_NetBridgeQueue.GetLogicTaskQueue(pBuffer, nBufferLen);
}
int CNetDriver2::PutPacketStream(const char *pBuffer, int nBufferLen)
{
	return g_NetBridgeQueue.PutNetTaskQueue(pBuffer, nBufferLen);
	//IF(NULL==pNetObject)
	//{
	//	return -1;
	//}

	//CSocketClient *pSocketClient = (CSocketClient*)pNetObject;

	//return pSocketClient->Send(pBuffer, nBufferLen);
}

void CNetDriver2::Init()
{	
	InitSocketLib();

	//公共队列
	new CNetBridgeQueue;

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


