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
}

CNetKernel::~CNetKernel()
{	
	Release();
}

void CNetKernel::Init()
{
	INITASSERT(!m_IOCP.Create());
	InitThread();
}

void CNetKernel::InitThread()
{
	m_hThreadLoop = (HANDLE)_beginthreadex(NULL, 0, ThreadLoop, this, 0, &m_uThreadLoop);	
	INITASSERT( 0== m_hThreadLoop);	
}

void CNetKernel::Release()
{
	ReleaseThread();
}

void CNetKernel::ReleaseThread()
{
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

bool CNetKernel::AddConnectSocket(const char* pConnectIP, USHORT nConnectPort, bool bAutoConnect)
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
	AddClientSocket(pSocketClient);
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

	m_lConnect.Add(&pSocketClient->m_lNode);
	m_lSocketClient.Add(&pSocketClient->m_lNode);
	m_HashSocketClient.insert(pair<unsigned int, CSocketClient*>(pSocketClient->GetKey(),pSocketClient));
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
		m_lSocketClient.Del(&pSocketClient->m_lNode);
		m_lConnect.Add(&pSocketClient->m_lNode);
		return;
	}

	CSocketAPI::Close(pSocketClient->m_nSocket);
	m_lSocketClient.Del(&pSocketClient->m_lNode);
	m_HashSocketClient.erase(pSocketClient->GetKey());

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
	if (m_HashSocketClient.end()==m_HashSocketClient.find(nSocketKey))
	{
		return false;
	}

	return true;
}

CSocketClient* CNetKernel::GetSocketClientByKey(unsigned int nSocketKey)
{
	HASH_SOCKETCLIENT::iterator ite = m_HashSocketClient.find(nSocketKey);

	if(ite==m_HashSocketClient.end())
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
	CSocketClient *pSocketClient;
	list_head *pListIte;

	list_for_each(pListIte,  &m_lSocketClient.m_lHead)
	{
		pSocketClient = list_entry_offset(pListIte, CSocketClient, m_lNode);
		//send
		IFn( -1==pSocketClient->FlushSend())
		{
			
		}

	}//end list_for_each
}

void CNetKernel::LoopConnect()
{
	list_head *pListIte;
	list_head *pListIteTemp;
	CSocketClient *pSocketClient;

	list_for_each_safe(pListIte, pListIteTemp, &m_lConnect.m_lHead)
	{
		pSocketClient = list_entry_offset(pListIte, CSocketClient, m_lNode);

		//虽然是无阻塞连接，但是不适合大量断连，服务端需要重连的场景
		if ( 0==pSocketClient->Connect() )
		{
			m_lConnect.Del(pListIte);
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