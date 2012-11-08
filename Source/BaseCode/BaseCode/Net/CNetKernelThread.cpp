#include "CNetKernelThread.h"
#include "NetCallBackFun.h"
#include "GlobalMacro.h"
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

bool CNetKernelThread::AddClientSocket(const char* pConnectIP, USHORT nConnectPort, bool bAutoConnect)
{
	CSocketClient* pSocketClient = MallocSocketClientObject();
	IFn(NULL==pSocketClient)
	{
		return false;
	}

	pSocketClient->m_nStepFlag = IOCP_STEP_CONNECT;
	pSocketClient->m_bAutoConnect = bAutoConnect;
	pSocketClient->m_nPort = nConnectPort;
	pSocketClient->m_nIP = inet_addr(pConnectIP);
	AddToClientSocketList(pSocketClient);
	return true;
}

void CNetKernelThread::AddToClientSocketList(CSocketClient *pSocketClient)
{
	IFn(NULL==pSocketClient)
	{
		return;
	}

	IFn(VerifySocketClientValid(pSocketClient))
	{
		return;
	}

	m_ListSocketClient.Add(&pSocketClient->m_List);

	//
	m_HashSocketClient.insert(pair<CSocketClient*, CSocketClient*>(pSocketClient,pSocketClient));
}

void CNetKernelThread::DelFromClientSocketList(CSocketClient *pSocketClient)
{
	IFn(NULL==pSocketClient)
	{
		return;
	}

	IFn( !VerifySocketClientValid(pSocketClient))
	{
		return;
	}

	m_ListSocketClient.Del(&pSocketClient->m_List);
	m_HashSocketClient.erase(pSocketClient);
}

void CNetKernelThread::Loop()
{
	LoopIOCP();
	LoopListSocketClient();
	LoopBridgeQueue();
}

void CNetKernelThread::LoopIOCP()
{
	bool bResult=true;
	while( true==bResult)
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
		pSocketClient->m_nStepFlag = IOCP_STEP_ERR;	
		return true;
	}
	
	switch (pSocketClient->m_nIOCPEvent)
	{
	case IOCP_EVENT_ACCEPT_CONNECT:
		DoSocketClientAccept(pSocketClient);
		break;

	case IOCP_EVENT_RECV_BIG:
		pSocketClient->m_nStepFlag = IOCP_STEP_RECV_COMPLATE;
		break;

	default:
		LOGNE("Warning, switch (pIOCPKey->nEvent) deault,nEvent:%d\n", pSocketClient->m_nIOCPEvent);
	}

	return true;
}

void CNetKernelThread::LoopListSocketClient()
{
	CSocketClient *pSocketClient;
	list_head *pListEach;

	list_for_each(pListEach,  &m_ListSocketClient.m_ListHead)
	{
		pSocketClient = list_entry_offset(pListEach, CSocketClient, m_List);		

		//recv step flag
		switch (pSocketClient->m_nStepFlag)
		{
		case IOCP_STEP_ERR:  //出错
			DoSocketClientErrAndNoticLogic(pSocketClient);
			continue;
			
			break;

		case IOCP_STEP_RECV_ING:   //投递中，只做下发送
			break;

		case IOCP_STEP_RECV_COMPLATE:  //投递完成
			DoSocketClientRecv(pSocketClient);
			break;

		case IOCP_STEP_CONNECT:  //需要连接
			DoSocketClientConnect(pSocketClient);
			break;
		default:
			LOGNE("Err, CNetKernelThread::LoopListSocketClient(). m_nStepFlag:%d\n", pSocketClient->m_nStepFlag);
		}

		//send
		IFn( -1==pSocketClient->FlushSend())
		{
			
		}

	}//end list_for_each
}

void CNetKernelThread::LoopBridgeQueue()
{
	int nResult=0;
	char BufferPacket[name_msg_packet::SOCKET_BUFF_SIZE*2];
	while (1)
	{
		memset(BufferPacket, 0, sizeof(BufferPacket));
		nResult = g_NetBridgeQueue.GetNetTaskQueue(BufferPacket, name_msg_packet::SOCKET_BUFF_SIZE);
		if (1==nResult || -1==nResult)
		{
			break;
		}

		DoBridgeQueue(BufferPacket);
	}
}

void CNetKernelThread::DoSocketClientRecv(CSocketClient *pSocketClient)
{
	IFn(NULL==pSocketClient)
	{
		return;
	}

	if ( 1==g_NetBridgeQueue.PutLogicTaskQueue(&pSocketClient->m_RecvBuffer) )
	{
		//根据回调返回，是否需要投递
		PostIOCPRecv(pSocketClient);
	}
}

void CNetKernelThread::DoSocketClientConnect(CSocketClient *pSocketClient)
{
	IFn(NULL==pSocketClient)
	{
		return;
	}
	/*
		@虽然是无阻塞连接，但是不适合大量断连，服务端需要重连的场景
	*/
	if ( 0==pSocketClient->Connect() )
	{
		PostIOCPRecv(pSocketClient);
	}
	
}

void CNetKernelThread::DoSocketClientAccept(CSocketClient *pSocketClient)
{
	IFn(NULL==pSocketClient)
	{
		return;
	}

	//加入链表
	AddToClientSocketList(pSocketClient);

	IFn( 0==m_IOCP.AssociateSocket(pSocketClient->m_nSocket,  (ULONG_PTR)pSocketClient) )
	{
		pSocketClient->m_nStepFlag = IOCP_STEP_ERR;
		return;
	}

	PostIOCPRecv(pSocketClient);
}

void CNetKernelThread::DoSocketClientErr(CSocketClient *pSocketClient)
{
	IFn(NULL==pSocketClient)
	{
		return;
	}

	if ( !VerifySocketClientValid(pSocketClient))
	{
		LOGNN("CNetKernelThread::DoSocketClientErr, Hash not find. %x\n", pSocketClient);
		return;
	}

	//判断m_bAutoConnect字段
	if (true==pSocketClient->m_bAutoConnect)
	{
		pSocketClient->m_nStepFlag = IOCP_STEP_CONNECT;
		return;
	}

	//先从本链表中删除
	DelFromClientSocketList(pSocketClient);

	//LOG记录
	in_addr addrIP;
	addrIP.S_un.S_addr = pSocketClient->m_nIP;
	LOGNN("Notice, DoSocketClientErr Close. Socket:%d, IP:%s, Port:%d", 
		pSocketClient->m_nSocket,
		inet_ntoa( addrIP ),
		pSocketClient->m_nPort
		);	

}

void CNetKernelThread::DoSocketClientErrAndNoticLogic(CSocketClient *pSocketClient)
{
	IFn(NULL==pSocketClient)
	{
		return;
	}

	//判断该对象是否还存在
	if (!VerifySocketClientValid(pSocketClient))
	{
		LOGNN("CNetKernelThread::DoSocketClientErrAndNoticLogic, Hash not find. %x\n", pSocketClient);
		return;
	}

	DoSocketClientErr(pSocketClient);

	//调用回调函数
	PNLInnerNotic msgInnerNotic;
	msgInnerNotic.SetPacketDefine2(PACKET2_NTOL_ERR);
	msgInnerNotic.SetNetObject( (void*)pSocketClient );

	if ( -1==g_NetBridgeQueue.PutLogicTaskQueue((char*)&msgInnerNotic, msgInnerNotic.GetPacketSize()) )
	{
		LOGNN("Notic, CNetKernelThread::DoSocketClientErr. -1==PutLogicTaskQueue\n");
	}
}

void CNetKernelThread::DoBridgeQueue(const char *pBuffer)
{
	IFn(NULL==pBuffer)
	{
		return;
	}

	IPackHead *pPackHead = (IPackHead*)pBuffer;
	if( PACKET1_INNER_NET_LOGIC==pPackHead->GetPacketDefine1() )
	{
		return DoBridgeNLInnerNotic((PNLInnerNotic*)pBuffer);
	}

	//
	CSocketClient *pSocketClient = (CSocketClient*)pPackHead->GetNetObject();
	IFn(NULL==pSocketClient)
	{
		return ;
	}

	//先检测pSocketClient还是否有效
	IFn( !VerifySocketClientValid(pSocketClient) )
	{
		return;
	}

	IFn(-1==pSocketClient->Send(pBuffer, pPackHead->GetPacketSize()) )
	{
	
	}
}

void CNetKernelThread::DoBridgeNLInnerNotic(PNLInnerNotic *pNLInnerNotic)
{
	IFn(NULL==pNLInnerNotic)
	{
		return;
	}

	switch(pNLInnerNotic->GetPacketDefine2())
	{
		// PACKET2_LTON_SERVER_SOCKET,  已直接转给CNetAcceptThread模块
	case PACKET2_LTON_CONNECT_SOCKET:
		in_addr addrIP;
		addrIP.S_un.S_addr = pNLInnerNotic->m_nIP;
		AddClientSocket(inet_ntoa( addrIP ), pNLInnerNotic->m_nPort, pNLInnerNotic->m_bAutoConnect);
		break;

	case PACKET2_LTON_ERR:
		DoSocketClientErr( (CSocketClient*)pNLInnerNotic->m_ErrParam);
		break;

	default:
		LOGNE("Err, CNetKernelThread::DoBridgeNLInnerNotic, case default:%d\n", pNLInnerNotic->GetPacketDefine2());

	}
}

void CNetKernelThread::PostIOCPRecv(CSocketClient *pSocketClient)
{
	IFn(NULL==pSocketClient)
	{
		return;
	}

	IFn(-1==pSocketClient->Recv())
	{
		pSocketClient->m_nStepFlag = IOCP_STEP_ERR;
	}
}

bool CNetKernelThread::VerifySocketClientValid(CSocketClient *pSocketClient)
{
	//判断该对象是否还存在
	if (m_HashSocketClient.end()==m_HashSocketClient.find(pSocketClient))
	{
		return false;
	}

	return true;
}

unsigned int WINAPI CNetKernelThread::ThreadLoop(void* pParam)
{
	CNetKernelThread *pNetDriver=(CNetKernelThread*)pParam;
	while(pNetDriver->m_bThreadRun)
	{
		pNetDriver->Loop();
		Sleep(200);
	}

	return 0;
}