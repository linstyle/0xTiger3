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
}

CNetDriver2::~CNetDriver2()
{
	Release();
}

bool CNetDriver2::SetSocketServer(const char* pName, const char* pListenIP, USHORT nListenPort)
{
	IF(m_bHasInit)
		return false;

	INITASSERT(!pName || !pListenIP);
	INITASSERT(!m_NetAccept.SetSocketServer(pName, pListenIP, nListenPort));
	
	m_bHasInit = true;
	return true;
}

bool CNetDriver2::AddConnectSocket(const char* pConnectIP, USHORT nConnectPort)
{
	IF(m_bHasInit)
		return false;

	INITASSERT(!pConnectIP);
	INITASSERT(!m_NetKernel.AddConnectSocket(pConnectIP, nConnectPort));

	m_bHasInit = true;
	return true;
}

bool CNetDriver2::CloseNet(CBasePlayer* pBasePlayer)
{
	IF(!pBasePlayer)
		return false;

	PInnerTransfer msgInnerTransfer;
	IF(!msgInnerTransfer.CreateLtoNErr( pBasePlayer->GetNetKey() ))
		return false;

	return SendPacket(&msgInnerTransfer, pBasePlayer);
}

bool CNetDriver2::SendPacket(IPacketHead* pPacketHead, CBasePlayer* pBasePlayer)
{
	IF(!pPacketHead || !pBasePlayer)
		return false;

	__try
	{
		return g_NLBridgeQueue.PutToNetQueue(pPacketHead);
	}
	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))
	{
		return false;
	}	
}

//int CNetDriver2::GetPacketStream(char *pBuffer, int nBufferLen)
//{
//	IF(!pBuffer)
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
	delete g_NLBridgeQueue.getSingletonPtr();
}

void CNetDriver2::ReleaseAllConnect()
{

}

CNetKernel* CNetDriver2::GetNetKerner()
{
	return &m_NetKernel;
}

