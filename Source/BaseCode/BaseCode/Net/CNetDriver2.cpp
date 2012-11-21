#include "CNetDriver2.h"
#include "GlobalMacro.h"
#include <process.h>
#include "PInnerTransfer.h"
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
	IFn(m_bHasInit)
		return false;

	INITASSERT(!pName || !pListenIP);
	INITASSERT(!m_NetAccept.SetSocketServer(pName, pListenIP, nListenPort));
	
	m_bHasInit = true;
	return true;
}

bool CNetDriver2::AddConnectSocket(const char* pConnectIP, USHORT nConnectPort)
{
	IFn(m_bHasInit)
		return false;

	INITASSERT(!pConnectIP);
	INITASSERT(!m_NetKernel.AddConnectSocket(pConnectIP, nConnectPort));

	m_bHasInit = true;
	return true;
}

bool CNetDriver2::CloseNet(unsigned int nNetKey)
{
	PInnerTransfer msgInnerTransfer;
	IFn(!msgInnerTransfer.CreateLtoNErr(nNetKey))
		return false;

	return SendPacket(&msgInnerTransfer);
}

bool CNetDriver2::SendPacket(IPacketHead* pPackHead)
{
	IFn(!pPackHead)
		return false;

	__try
	{
		return g_NLBridgeQueue.PutToNetQueue(pPackHead);
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

