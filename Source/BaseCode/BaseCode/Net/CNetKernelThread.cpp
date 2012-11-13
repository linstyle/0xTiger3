#include "CNetKernelThread.h"
#include "NetCallBackFun.h"
#include "GlobalMacro.h"
#include "CSocketAPI.h"
#include <process.h>

CNetKernelThread::CNetKernelThread()
{
	m_bThreadRun = true;
	m_hThreadLoop = NULL;
	m_uThreadLoop = 0;
}

CNetKernelThread::~CNetKernelThread()
{	
	Release();
}

void CNetKernelThread::Init()
{
	INITASSERT(!m_IOCP.Create());
	InitThread();
}

void CNetKernelThread::InitThread()
{
	m_hThreadLoop = (HANDLE)_beginthreadex(NULL, 0, ThreadLoop, this, 0, &m_uThreadLoop);	
	INITASSERT( 0== m_hThreadLoop);	
}

void CNetKernelThread::Release()
{
	ReleaseThread();
}

void CNetKernelThread::ReleaseThread()
{
	m_bThreadRun = false;

	IFn( WAIT_FAILED==WaitForSingleObject(m_hThreadLoop, INFINITE) )
	{

	}
	CloseHandle(m_hThreadLoop);
}

CIOCP* CNetKernelThread::GetIOCP()
{
	return &m_IOCP;
}

bool CNetKernelThread::AddConnectSocket(const char* pConnectIP, USHORT nConnectPort, bool bAutoConnect)
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

void CNetKernelThread::AddClientSocket(CSocketClient *pSocketClient)
{
	IFn(NULL==pSocketClient)
	{
		return;
	}

	m_lSocketClient.Add(&pSocketClient->m_lNode);
	m_HashSocketClient.insert(pair<unsigned int, CSocketClient*>(pSocketClient->GetKey(),pSocketClient));
}

void CNetKernelThread::CloseClientSocket(CSocketClient *pSocketClient,bool bNotifyLogic/*=true*/)
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

	}
}

bool CNetKernelThread::VerifySocketClientValid(unsigned int nSocketKey)
{
	//判断该对象是否还存在
	if (m_HashSocketClient.end()==m_HashSocketClient.find(nSocketKey))
	{
		return false;
	}

	return true;
}

CSocketClient* CNetKernelThread::GetSocketClientByKey(unsigned int nSocketKey)
{
	HASH_SOCKETCLIENT::iterator ite = m_HashSocketClient.find(nSocketKey);

	if(ite==m_HashSocketClient.end())
	{
		return NULL;
	}

	return ite->second;
}

void CNetKernelThread::Loop()
{
	__try
	{
		LoopIOCP();
		LoopSendData();
		LoopConnect();	
	}
	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))	
	{
		return;
	}

}

void CNetKernelThread::LoopIOCP()
{
	bool bResult=true;

	while( bResult)
	{
		bResult = _LoopIOCP();
	}	
}

bool CNetKernelThread::_LoopIOCP()
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

void CNetKernelThread::LoopSendData()
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

void CNetKernelThread::LoopConnect()
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

void CNetKernelThread::LoopBridgeQueue()
{
	int nResult=0;
	char BufferPacket[name_msg_packet::SOCKET_BUFF_SIZE*2];
	const char* pBuffer = BufferPacket;

	while (1)
	{
		memset(BufferPacket, 0, sizeof(BufferPacket));
		nResult = g_NetBridgeQueue.GetNetTaskQueue(BufferPacket, name_msg_packet::SOCKET_BUFF_SIZE);
		if (1==nResult || -1==nResult)
		{
			break;
		}

		IPackHead *pPackHead = (IPackHead*)pBuffer;
		CSocketClient *pSocketClient = GetSocketClientByKey(pPackHead->GetNetKey());
		IFn(NULL==pSocketClient)
		{
			continue;
		}

		//逻辑层和网络层内部的包
		if( PACKET1_INNER_NET_LOGIC==pPackHead->GetPacketDefine1() )
		{
			g_PacketFactory.ProcessMsg(pPackHead);			
			continue;
		}

		IFn(-1==pSocketClient->Send(pBuffer, pPackHead->GetPacketSize()) )
		{
			CloseClientSocket(pSocketClient);
			continue;
		}
	}
}

void CNetKernelThread::OnAcceptSocket(CSocketClient *pSocketClient)
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

void CNetKernelThread::OnRecvSocket(CSocketClient *pSocketClient)
{
	IFn(NULL==pSocketClient)
	{
		return;
	}

	while (0==g_NetBridgeQueue.PutLogicTaskQueue(&pSocketClient->m_RecvBuffer))
	{
		
	}

	IFn(-1==pSocketClient->Recv())
	{
		CloseClientSocket(pSocketClient);
	}
}

unsigned int WINAPI CNetKernelThread::ThreadLoop(void* pParam)
{
	IFn(!pParam)
		return 0;

	CNetKernelThread *pNetDriver=(CNetKernelThread*)pParam;
	while(pNetDriver->m_bThreadRun)
	{
		pNetDriver->Loop();
		Sleep(200);
	}

	return 0;
}