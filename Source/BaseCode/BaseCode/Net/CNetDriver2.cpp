#include "CNetDriver2.h"
#include "GlobalMacro.h"
#include <process.h>
#include "PInnerTransfer.h"
initialiseSingleton(CNetDriver2);


CNetDriver2::CNetDriver2()
{
	//��������
	new CNLBridgeQueue;

	m_bHasInit = false;
}

CNetDriver2::~CNetDriver2()
{
	Release();
}

bool CNetDriver2::CreateServerSocket(const char* pName, const char* pListenIP, USHORT nListenPort)
{
	INITASSERT(!pName || !pListenIP);
	INITASSERT(!m_NetAccept.CreateServerSocket(pName, pListenIP, nListenPort));
	
	m_bHasInit = true;
	return true;
}

bool CNetDriver2::CreateConnectSocket(const char* pConnectIP, USHORT nConnectPort)
{
	INITASSERT(!pConnectIP);
	INITASSERT(!m_NetKernel.CreateConnectSocket(pConnectIP, nConnectPort));

	m_bHasInit = true;
	return true;
}

bool CNetDriver2::CloseNet(CBasePlayer* pBasePlayer)
{
	IF(!pBasePlayer)
		return false;

	PInnerTransferToNet msgInnerTransfer;
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

