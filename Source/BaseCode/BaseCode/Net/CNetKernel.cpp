#include "CNetKernel.h"
#include "CNLBridgeQueue.h"
#include "GlobalMacro.h"
#include "GlobalFunction.h"
#include "CSocketAPI.h"
#include "CPacketFactory.h"
#include "PInnerTransfer.h"
#include <process.h>


CNetKernel::CNetKernel()
{
	m_bThreadRun = true;
	m_hThreadLoop = NULL;
	m_uThreadLoop = 0;
	m_bHasInitSocket = false;
}

CNetKernel::~CNetKernel()
{	
	Release();
}

void CNetKernel::Init()
{
	INITASSERT(!m_IOCP.Create());

	if (m_bHasInitSocket)
	{
		InitThread();
	}
	
}

void CNetKernel::InitThread()
{
	m_hThreadLoop = (HANDLE)_beginthreadex(NULL, 0, ThreadLoop, this, 0, &m_uThreadLoop);	
	INITASSERT( 0== m_hThreadLoop);	
}

void CNetKernel::Release()
{
	ReleaseThread();
	ReleaseSocket();
}

void CNetKernel::ReleaseSocket()
{
	CSocketClient* pSocketClient = NULL;
	MAP_SOCKETCLIENT::iterator ite;
	for( ite=m_mapAllClientSocket.begin(); ite!=m_mapAllClientSocket.end(); )
	{
		pSocketClient = ite->second;
		pSocketClient->m_bAutoConnect = false;
		CloseClientSocket(pSocketClient);
	}

	list_head *pListIte;
	list_head *pListIteTemp;
	list_for_each_safe(pListIte, pListIteTemp, &m_lstConnect.m_lHead)
	{
		pSocketClient = list_entry_offset(pListIte, CSocketClient, m_lConnectNode);

		pSocketClient->m_bAutoConnect = false;
		FreeSocketClientObject(pSocketClient);
	}
}

void CNetKernel::ReleaseThread()
{
	if (!m_hThreadLoop)
	{
		return;
	}

	m_bThreadRun = false;

	IFn( WAIT_FAILED==WaitForSingleObject(m_hThreadLoop, INFINITE) )
	{

	}
	CloseHandle(m_hThreadLoop);
}

CIOCP* CNetKernel::GetIOCP()
{
	return &m_IOCP;
}

bool CNetKernel::CreateConnectSocket(const char* pConnectIP, USHORT nConnectPort, bool bAutoConnect)
{
	IFn(!pConnectIP)
		return false;

	CSocketClient* pSocketClient = MallocSocketClientObject();
	IFn(NULL==pSocketClient)
	{
		return false;
	}

	pSocketClient->m_bAutoConnect = bAutoConnect;
	pSocketClient->m_nPort = nConnectPort;
	pSocketClient->m_nIP = inet_addr(pConnectIP);
	m_lstConnect.Add(&pSocketClient->m_lConnectNode);

	m_bHasInitSocket = true;

	return true;
}

bool CNetKernel::SendToBufferByNetKey(IPacketHead* pPacketHead, unsigned int nNetKey)
{
	CSocketClient *pSocketClient = GetSocketClientByKey(nNetKey);
	if(NULL==pSocketClient)
	{
		LOGE("CNetKernel::SendToBufferByNetKey.pSocketClient NULL Point.nNetKey:%d", nNetKey);
		return false;
	}

	IFn(!pPacketHead)
		return false;


	if(!pSocketClient->Send(pPacketHead->GetPacketBuffer(), pPacketHead->GetPacketSize()) )
	{
		//包队列满了，不一定要关闭
		//CloseClientSocket(pSocketClient);
		LOGE("CNetKernel::SendToBufferByNetKey.Buffer Over.PacketDefin1:%d, PacketDefin2:%d\n", 
			pPacketHead->GetPacketDefine1(), pPacketHead->GetPacketDefine2());
		return false;
	}	

	return true;
}

void CNetKernel::CloseClientSocketByNetKey(unsigned int nNetKey)
{
	CSocketClient* pSocketClient = GetSocketClientByKey(nNetKey);
	if(!pSocketClient)
	{
		LOGNE("CNetKernel::CloseClientSocketByNetKey.if(!pSocketClient).nNetKey:%d\n",nNetKey);
		return;
	}

	CloseClientSocket(pSocketClient, false);
}

void CNetKernel::AddClientSocket(CSocketClient *pSocketClient)
{
	IFn(NULL==pSocketClient)
	{
		return;
	}

	m_mapAllClientSocket.insert(pair<unsigned int, CSocketClient*>(pSocketClient->GetKey(),pSocketClient));
}

void CNetKernel::CloseClientSocket(CSocketClient *pSocketClient,bool bNotifyLogic/*=true*/)
{
	IFn(NULL==pSocketClient)
	{
		return;
	}

	IFn( !VerifySocketClientValid(pSocketClient->GetKey()) )
	{
		return;
	}

	//LOG记录
	in_addr addrIP;
	addrIP.S_un.S_addr = pSocketClient->m_nIP;
	LOGNN("Notice, CloseClientSocket. Socket:%d, IP:%s, Port:%d, AutoConnect:%d", 
		pSocketClient->m_nSocket,
		inet_ntoa( addrIP ),
		pSocketClient->m_nPort,
		pSocketClient->m_bAutoConnect
		);	

	if(pSocketClient->m_bAutoConnect)
	{
		m_mapAllClientSocket.erase(pSocketClient->GetKey());
		m_lstConnect.Add(&pSocketClient->m_lConnectNode);
		return;
	}

	CSocketAPI::Close(pSocketClient->m_nSocket);
	

	//最后Free掉
	FreeSocketClientObject(pSocketClient);

	//通知逻辑层，表示这个错误是网络层主动发起的
	if(bNotifyLogic)
	{
		PInnerTransfer msgInnerTransfer;
		IFn( !msgInnerTransfer.CreateNtoLErr(pSocketClient->GetKey()) )
		{
			return;
		}

		if (!g_NLBridgeQueue.PutToLogicQueue(&msgInnerTransfer))
		{
			LOGNE("CNetKernel::CloseClientSocket.nNetKey:%d\n", pSocketClient->GetKey());
		}
	}
}

bool CNetKernel::VerifySocketClientValid(unsigned int nSocketKey)
{
	//判断该对象是否还存在
	if (m_mapAllClientSocket.end()==m_mapAllClientSocket.find(nSocketKey))
	{
		return false;
	}

	return true;
}

CSocketClient* CNetKernel::GetSocketClientByKey(unsigned int nSocketKey)
{
	MAP_SOCKETCLIENT::iterator ite = m_mapAllClientSocket.find(nSocketKey);

	if(ite==m_mapAllClientSocket.end())
	{
		return NULL;
	}

	return ite->second;
}

void CNetKernel::Loop()
{
	__try
	{
		LoopIOCP();
		LoopConnect();
		LoopSendData();			
		LoopBridgeQueue();
	}
	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))	
	{
		return;
	}

}

void CNetKernel::LoopIOCP()
{
	bool bResult=true;

	while( bResult)
	{
		bResult = _LoopIOCP();
	}	
}

bool CNetKernel::_LoopIOCP()
{
	BOOL bResult;
	DWORD dwNumBytes;
	CSocketClient *pSocketClient;
	OVERLAPPED *pOverLapped;

	/*
		[in] Number of milliseconds that the caller is willing to wait for an completion packet to appear at the 
		completion port. If a completion packet does not appear within the specified time, the function times out, 
		returns FALSE, and sets *lpOverlapped to NULL. 	
	*/
	bResult = m_IOCP.GetStatus( (ULONG_PTR*)&pSocketClient, &dwNumBytes, &pOverLapped, 0);
	if( false==bResult && NULL==pOverLapped)    
	{
		//time out
		return false;
	}

	if( 0==dwNumBytes || (false==bResult && NULL!=pOverLapped) )
	{
		CloseClientSocket(pSocketClient);	
		return true;
	}
	
	switch (pSocketClient->m_nIOCPEvent)
	{
	case IOCP_EVENT_ACCEPT_CONNECT:
		OnAcceptSocket(pSocketClient);
		break;

	case IOCP_EVENT_RECV_BIG:
		OnRecvSocket(pSocketClient);
		break;

	default:
		LOGNE("Warning, switch (pIOCPKey->nEvent) deault,nEvent:%d\n", pSocketClient->m_nIOCPEvent);
	}

	return true;
}

void CNetKernel::LoopSendData()
{
	CSocketClient *pSocketClient = NULL;
	MAP_SOCKETCLIENT::iterator ite;

	for( ite=m_mapAllClientSocket.begin(); ite!=m_mapAllClientSocket.end(); )
	{
		pSocketClient = ite->second;
		IFn( -1==pSocketClient->FlushSend())
		{
			CloseClientSocket(pSocketClient);
			ite = m_mapAllClientSocket.erase(ite);
		}
		else
		{
			ite++;
		}
	}
}

void CNetKernel::LoopConnect()
{
	list_head *pListIte;
	list_head *pListIteTemp;
	CSocketClient *pSocketClient;

	list_for_each_safe(pListIte, pListIteTemp, &m_lstConnect.m_lHead)
	{
		pSocketClient = list_entry_offset(pListIte, CSocketClient, m_lConnectNode);

		//虽然是无阻塞连接，但是不适合大量断连，服务端需要重连的场景
		if ( 0==pSocketClient->Connect() )
		{
			m_lstConnect.Del(pListIte);
			AddClientSocket(pSocketClient);
			//连接成功
			IFn(-1==pSocketClient->Recv())
			{				
				CloseClientSocket(pSocketClient);
			}			
		}
	}
}

void CNetKernel::LoopBridgeQueue()
{
	char BufferPacket[NET_PACKET_BUFF_SIZE];
	const char* pBuffer = BufferPacket;

	while (1)
	{
		MEMSET(&BufferPacket, 0, sizeof(BufferPacket));
		if ( !g_NLBridgeQueue.GetFromNetQueue(BufferPacket, NET_PACKET_BUFF_SIZE) )
		{
			break;
		}
		
		IPacketHead* pPacketHead = (IPacketHead*)pBuffer;

		g_PacketFactory.ProcessMsg(pPacketHead);

	}
}

void CNetKernel::OnAcceptSocket(CSocketClient *pSocketClient)
{
	IFn(NULL==pSocketClient)
	{
		return;
	}

	//加入链表
	AddClientSocket(pSocketClient);

	IFn( 0==m_IOCP.AssociateSocket(pSocketClient->m_nSocket,  (ULONG_PTR)pSocketClient) )
	{
		CloseClientSocket(pSocketClient);
	}

	IFn(-1==pSocketClient->Recv())
	{
		CloseClientSocket(pSocketClient);
	}
}

void CNetKernel::OnRecvSocket(CSocketClient *pSocketClient)
{
	IFn(NULL==pSocketClient)
	{
		return;
	}

	CCircleBuffer* pCircleBuffer = &pSocketClient->m_RecvBuffer;
	while(1)
	{
		PInnerTransfer msgInnerTransfer;

		//读头部数据大小字段
		if( !pCircleBuffer->TryReadBuffer((char*)&msgInnerTransfer,  NET_PACKET_HEAD_SIZE) )
		{
			break;
		}

		IFn (NET_PACKET_BUFF_SIZE < msgInnerTransfer.GetPacketSize())
		{
			break;
		}

		if (!pCircleBuffer->TryReadBuffer(msgInnerTransfer.GetPacketBuffer(), msgInnerTransfer.GetPacketSize()) )
		{
			break;
		}

		msgInnerTransfer.CreateNtoL(pSocketClient->GetKey());
		
		if (!g_NLBridgeQueue.PutToLogicQueue(&msgInnerTransfer))
		{
			break;
		}

		pCircleBuffer->ReadBufferFlush(msgInnerTransfer.GetPacketSize());		
	}

	IFn(-1==pSocketClient->Recv())
	{
		CloseClientSocket(pSocketClient);
	}
}



unsigned int WINAPI CNetKernel::ThreadLoop(void* pParam)
{
	IFn(!pParam)
		return 0;

	CNetKernel *pNetDriver=(CNetKernel*)pParam;
	while(pNetDriver->m_bThreadRun)
	{
		pNetDriver->Loop();
		Sleep(100);
	}

	return 0;
}