#include "CNetDriver2.h"
#include "GlobalMacro.h"
#include <process.h>
initialiseSingleton(CNetDriver2);


CNetDriver2::CNetDriver2()
{
	//公共队列
	new CNLBridgeQueue;

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

	INITASSERT(!m_NetAccept.SetSocketServer(pName, pListenIP, nListenPort));
	
	m_bHasInit = true;
	return true;
}

bool CNetDriver2::AddConnectSocket(const char* pConnectIP, USHORT nConnectPort)
{
	//IFn(!pConnectIP)
	//	return false;

	//PNLInnerNotic msgInnerNotic;
	//msgInnerNotic.SetPacketDefine2( PACKET2_LTON_CONNECT_SOCKET);
	//msgInnerNotic.m_nIP = inet_addr(pConnectIP);
	//msgInnerNotic.m_nPort = nConnectPort;

	//IFn (-1==g_NLBridgeQueue.PutNetTaskQueue((char*)&msgInnerNotic, msgInnerNotic.GetPacketSize()))
	//{
	//	return false;
	//}

	//m_bHasInit = true;
	return true;
}

int CNetDriver2::CloseNet(unsigned int nNetKey)
{
	//PNLInnerNotic msgInnerNotic;
	//msgInnerNotic.SetPacketDefine2( PACKET2_LTON_ERR);
	//msgInnerNotic.SetNetKey(nNetKey);

	//int nResult = g_NLBridgeQueue.PutNetTaskQueue((char*)&msgInnerNotic, msgInnerNotic.GetPacketSize());
	//
	//if(-1==nResult)
	//{
	//	LOGNE("CNetDriver2::CloseNet,-1==PutNetTaskQueue.nNetKey:%d\n", nNetKey);
	//}

	//return nResult;
	return 0;
}

int CNetDriver2::SendPacket(IPacketHead* pPackHead)
{
	IFn(!pPackHead)
		return -1;

	__try
	{
		return g_NLBridgeQueue.PutNetTaskQueue(pPackHead);
	}
	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))
	{
		return false;
	}	
}

//int CNetDriver2::GetPacketStream(char *pBuffer, int nBufferLen)
//{
//	IFn(!pBuffer)
//		return -1;
//
//	__try
//	{
//		return g_NLBridgeQueue.GetLogicTaskQueue(pBuffer, nBufferLen);
//	}
//	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))
//	{
//		return false;
//	}	
//	
//}

void CNetDriver2::Init()
{	
	INITASSERT(!m_bHasInit);

	m_NetKernel.Init();
	m_NetAccept.Init(m_NetKernel.GetIOCP());
}

void CNetDriver2::Release()
{
	ReleaseAllConnect();
	ReleaseSocketLib();

	delete g_NLBridgeQueue.getSingletonPtr();
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

CNetKernel* CNetDriver2::GetNetKerner()
{
	return &m_NetKernel;
}

