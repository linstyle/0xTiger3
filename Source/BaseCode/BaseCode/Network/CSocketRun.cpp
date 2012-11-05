#include "StdAfx.h"
#include "CSocketRun.h"

CSocketRun::~CSocketRun()
{
	m_bThreadRun = false;
	CloseAllSocket();
	EndThreadAccept();
}

void CSocketRun::Init(ILocalObjectConnect* pObjectConnect, int nConnectCount, 
			  ILocalObjectAccept* pObjectAccept, int nAcceptCount)
{
	m_bThreadRun = true;
	InitConnect(pObjectConnect, nConnectCount);
	InitAccept(pObjectAccept, nAcceptCount);
	InitIOCP();

	StartThreadAccept();
}
void CSocketRun::TracerMe()
{
	printf("Send Socket Count:%d\n", m_ListSendActive.GetCount());
	printf("Recv Socket Count:%d\n", m_ListRecvActive.GetCount());
	printf("All Socket Count:%d\n", m_ListAllSocket.GetCount());
}

bool CSocketRun::InitIOCP()
{
	int nResult;

	nResult = m_IOCP.Create();	
	INITASSERT( 0==nResult );

	return true;
}

bool CSocketRun::InitConnect(ILocalObjectConnect* pObjectConnect, int nConnectCount)
{
	for(int i=0; i<nConnectCount; ++i)
	{
		AddLocalConnect(&pObjectConnect[i]);
	}

	return true;
}
bool CSocketRun::InitAccept(ILocalObjectAccept* pObjectAccept, int nAcceptCount)
{
	for(int i=0; i<nAcceptCount; ++i)
	{
		AddLocalAccept(&pObjectAccept[i]);
	}
	return true;
}

void CSocketRun::Run()
{
	FlushRecv();
	FlushSend();
	FlushIOCP();
	FlushConnect();
}

int CSocketRun::Send(CIOCPData *pIOCPKey, IPackHead *pPackHead)
{
	AddSend(pIOCPKey);
	if( -1==pIOCPKey->Send(pPackHead))
	{
		CloseSocket(pIOCPKey);
		printf("Err, CSocketRun::Send,pIOCPKey(%x)\n", pIOCPKey);
		return -1;
	}
	
	return 0;
}

/*
	对链表的操作
*/
void CSocketRun::FlushRecv()
{
	int nResult, nPacketContentLen;
	CIOCPData *pIOCPKey;	
	list_head *pHead, *pEach, *pSafe;

	pHead = GetListRecv();
	
	list_for_each_safe(pEach, pSafe, pHead)
	{
		pIOCPKey = list_entry_offset(pEach, CIOCPData, m_ListRecv);		
		nResult = pIOCPKey->RecvPacketHead();
		if( -1==nResult )
		{
			//表示空间不够
			ResetRecv(pIOCPKey);
			continue;
		}

		nPacketContentLen = pIOCPKey->InitRecvPacketContent();
		if( -1==nPacketContentLen )
		{			
			CloseSocket(pIOCPKey);
			continue;
		}
		nResult = pIOCPKey->RecvPacketContent(nPacketContentLen);
		if( -1==nResult )
		{		
			//长度不够
			ResetRecv(pIOCPKey);
			continue;
		}

		pIOCPKey->RecvBuffer.ReadBufferFlush(nPacketContentLen+name_msg_packet::PACKET_HEAD_SIZE);
		
		/*
			调试信息
		*/
		++g_NetTracer.m_nRecv;
		++g_NetTracer.m_nRecvSec;		
		pIOCPKey->RecvBuffer.TracerMe();

		if( -1==ExcutePacket(pIOCPKey))
		{
			//CloseSocketAndLogic(pIOCPKey);
			//返回出错，表示逻辑层已经关闭该对象
			CloseSocket(pIOCPKey);
			continue;
		}

		if( pIOCPKey->RecvBuffer.GetUseLength()<=20 )//判断当前有效长度,20我随便写的
		{
			ResetRecv(pIOCPKey);		
		}
	}
}
void CSocketRun::FlushSend()
{
	int nResult;
	CIOCPData *pIOCPKey;
	list_head *pHead, *pEach, *pSafe;

	pHead = GetListSend();
	
	list_for_each_safe(pEach, pSafe, pHead)
	{
		pIOCPKey = list_entry_offset(pEach, CIOCPData, m_ListSend);

		nResult = pIOCPKey->FlushSend();
		if( -1==nResult ) //出错
		{
			printf("Err, CSocketRun::FlushSend():This(%x)\n", this);
			CloseSocket(pIOCPKey);
		}
		if( 0==nResult)  //都发送完成
		{
			DelSend(pIOCPKey);
		}
		
		/*
			调试信息
		*/
		pIOCPKey->SendBuffer.TracerMe();
		++g_NetTracer.m_nSend;
		++g_NetTracer.m_nSendSec;
	}
}

void CSocketRun::FlushIOCP()
{
	bool bResult=true;
	while( true==bResult)
	{
		bResult = _FlushIOCP();
	}		
}

bool CSocketRun::_FlushIOCP()
{
	BOOL bResult;
	DWORD dwNumBytes;
	CIOCPData* pIOCPKey;
	OVERLAPPED *pOverLapped;

	/*
		[in] Number of milliseconds that the caller is willing to wait for an completion packet to appear at the 
		completion port. If a completion packet does not appear within the specified time, the function times out, 
		returns FALSE, and sets *lpOverlapped to NULL. 	
	*/
	bResult = m_IOCP.GetStatus( (ULONG_PTR*)&pIOCPKey, &dwNumBytes, &pOverLapped, 0);
	if( false==bResult && NULL==pOverLapped)    
	{
		//time out
		return false;
	}

	//表示已从投递链上收回
	DelIOCPPost(pIOCPKey);
	if( 0==dwNumBytes || (false==bResult && NULL!=pOverLapped) )
	{
		CloseSocket(pIOCPKey);
		printf("Notic, CSocketRun::_FlushIOCP:GetClose:this(%x)\n", pIOCPKey);			
		return true;
	}
	
	switch (pIOCPKey->nEvent)
	{
	case IOCP_EVENT_ACCEPT_CONNECT:
		//printf("IOCP_EVENT_ACCEPT_CONNECT\n");
		printf("Notic, CSocketRun::_FlushIOCP:Get Accpt/Connect Event:this(%x)\n", pIOCPKey);
		DoAcceptConnect(pIOCPKey);
		break;

	case IOCP_EVENT_RECV_BIG:
		//printf("Notic, CSocketRun::_FlushIOCP:Get Recv Event:this(%x)\n", pIOCPKey);
		pIOCPKey->FlushRecv(dwNumBytes);
		AddRecv(pIOCPKey);
		
		break;

	default:
		printf("Warning, switch (pIOCPKey->nEvent) deault,nEvent:%d\n", pIOCPKey->nEvent);
	}

	return true;
}

void CSocketRun::FlushConnect()
{
	int nCount = m_LocalObjectConnect.size();
	for(int i=0; i<nCount; ++i)
	{
		_FlushConnect(i);
	}
}
void CSocketRun::_FlushConnect(int nIndex)
{
	ILocalObjectConnect* pLocalConnect = m_LocalObjectConnect[nIndex];
	CIOCPData *pIOCPData = pLocalConnect->GetIOCPData();

	int nResult = pLocalConnect->Connect();
	if( -1==nResult )
	{
		printf("Err, CSocketRun::_FlushConnect:ErrID(%d), Name:%s\n", WSAGetLastError(), pLocalConnect->GetName());
		return;
	}
	else if( 1==nResult)
	{
		//已经是连接状态
		return;
	}
	else
	{
		printf("Notic, CSocketRun::_FlushConnect:OK:%d\n", pLocalConnect->GetSocket());

		pIOCPData->nSocket = pLocalConnect->GetSocket();				

		IOCPPostConnect(pIOCPData);
	}
}

int CSocketRun::AddSend(CIOCPData *pIOCPKey)
{
	if(true==pIOCPKey->BelongToList.IsSendList())
	{
		return 0;
	}

	pIOCPKey->BelongToList.SetSendList(true);
	m_ListSendActive.Add(&pIOCPKey->m_ListSend);
	return 0;
}
int CSocketRun::DelSend(CIOCPData *pIOCPKey)
{
	if(false==pIOCPKey->BelongToList.IsSendList())
	{
		return 0;
	}

	pIOCPKey->BelongToList.SetSendList(false);
	m_ListSendActive.Del(&pIOCPKey->m_ListSend);
	return 0;
}

int CSocketRun::AddRecv(CIOCPData *pIOCPKey)
{
	if(true==pIOCPKey->BelongToList.IsRecvList())
	{
		return 0;
	}

	pIOCPKey->BelongToList.SetRecvList(true);
	m_ListRecvActive.Add(&pIOCPKey->m_ListRecv);
	return 0;
}
int CSocketRun::DelRecv(CIOCPData *pIOCPKey)
{
	if(false==pIOCPKey->BelongToList.IsRecvList())
	{
		return 0;
	}

	pIOCPKey->BelongToList.SetRecvList(false);
	m_ListRecvActive.Del(&pIOCPKey->m_ListRecv);
	return 0;
}
void CSocketRun::ResetRecv(CIOCPData *pIOCPKey)
{
	//先从链表中删除
	DelRecv(pIOCPKey);
	//投递一个IOCP
	IOCPPostRecv(pIOCPKey);
}

int CSocketRun::AddIOCPPost(CIOCPData *pIOCPKey)
{
	assert(false==pIOCPKey->BelongToList.IsIOCPPost());
	pIOCPKey->BelongToList.SetIOCPPost(true);
	
	return 0;	
}
int CSocketRun::DelIOCPPost(CIOCPData *pIOCPKey)
{
	assert(true==pIOCPKey->BelongToList.IsIOCPPost());
	pIOCPKey->BelongToList.SetIOCPPost(false);

	return 0;
}

void CSocketRun::IOCPPostConnect(CIOCPData *pIOCPKey)	//投递一个IOCP连接事件
{
	AddIOCPPost(pIOCPKey);
	if( FALSE==m_IOCP.PostStatus((ULONG_PTR)pIOCPKey, 1) )
	{
		printf("Err, CSocketRun::IOCPPostConnect,ErrID(%d)\n", WSAGetLastError());
		CloseSocket(pIOCPKey);
	}
}

void CSocketRun::IOCPPostRecv(CIOCPData *pIOCPKey)		//投递一个IOCP接收事件
{
	AddIOCPPost(pIOCPKey);
	if( -1==pIOCPKey->Recv() )
	{
		printf("Err, CSocketRun::IOCPPostRecv:Recv Failed:pIOCPKey(%x)\n", pIOCPKey);
		CloseSocket(pIOCPKey);
	}
}

int CSocketRun::AddAllSocket(CIOCPData *pIOCPKey)
{
	assert(false==pIOCPKey->BelongToList.IsAllSocketList());
	pIOCPKey->BelongToList.SetAllSocketList(true);

	m_ListAllSocket.Add(&pIOCPKey->m_ListAllSocket);
	return 0;
}

int CSocketRun::DelAllSocket(CIOCPData *pIOCPKey)
{
	assert(true==pIOCPKey->BelongToList.IsAllSocketList());
	pIOCPKey->BelongToList.SetAllSocketList(false);

	m_ListAllSocket.Del(&pIOCPKey->m_ListAllSocket);
	return 0;
}

int CSocketRun::AddLocalConnect(ILocalObjectConnect* pLocalObject)
{
	m_LocalObjectConnect.push_back(pLocalObject);
	return 0;
}
void CSocketRun::DoAcceptConnect(CIOCPData *pIOCPKey)
{
	++g_NetTracer.m_nAccept;
	printf("Notic,Accept ok,%d\n", pIOCPKey->nSocket);

	/*
		要注意，如果是连接后断开，这个时候对象是否被释放掉？
		如果自动重连，对上层的逻辑对象没有影响，那么这里的AcceptConnect
		就会有问题(建议是断开后，对象都被释放，然后重新申请一遍)
	*/
	AddAllSocket(pIOCPKey);
	if( -1==g_NetTransToLogic.AcceptConnect(pIOCPKey) )
	{
		CloseSocket(pIOCPKey);
		return ;
	}
	if( 0==m_IOCP.AssociateSocket(pIOCPKey->nSocket, (ULONG_PTR)pIOCPKey) )
	{
		printf("Err, CSocketRun::DoLocalConnect, SocketNum:%d, ErrNum:%d\n", pIOCPKey->nSocket, WSAGetLastError());
		CloseSocket(pIOCPKey);
		return;
	}

	AddIOCPPost(pIOCPKey);
	if( -1==pIOCPKey->Recv() )
	{
		CloseSocket(pIOCPKey);
	}
	printf("Notic, CSocketRun::DoAcceptConnect:Post Recv:this(%x)\n", pIOCPKey);
}
/*
	执行上层包
*/
int CSocketRun::ExcutePacket(CIOCPData* pIOCPKey)
{
	return g_NetTransToLogic.ExcutePacket(pIOCPKey);
}

void CSocketRun::CloseSocket(CIOCPData *pIOCPKey)
{
	if(true==pIOCPKey->BelongToList.IsSendList())
	{
		DelSend(pIOCPKey);
	}
	if(true==pIOCPKey->BelongToList.IsRecvList())
	{
		DelRecv(pIOCPKey);
	}
	if(true==pIOCPKey->BelongToList.IsIOCPPost())
	{
		DelIOCPPost(pIOCPKey);
	}
	if(true==pIOCPKey->BelongToList.IsAllSocketList())
	{
		DelAllSocket(pIOCPKey);
	}

	//判断是否本地套接字
	if(0==CloseLocalSocket(pIOCPKey))
	{
		return;
	}

	//否则调用逻辑关闭接口
	g_NetTransToLogic.ExceptionPacket(pIOCPKey);

	//释放资源
	FreeNetObject(pIOCPKey);
}

//void CSocketRun::CloseSocketAndLogic(CIOCPData *pIOCPKey)
//{
//	CloseSocket(pIOCPKey);
//	g_NetTransToLogic.ExceptionPacket(pIOCPKey);
//}
int CSocketRun::CloseLocalSocket(CIOCPData* pIOCPKey)
{
	int nCount = m_LocalObjectConnect.size();

	for(int i=0; i<nCount; ++i)
	{
		/*
			因为本地对象的CIOCPData是固定的，可以用来判断
		*/
		if( m_LocalObjectConnect[i]->GetIOCPData()==pIOCPKey )
		{
			m_LocalObjectConnect[i]->ResetConnect();
			return 0;
		}
	}

	return -1;
}

/*
	关闭所有套接字资源
*/
void CSocketRun::CloseAllSocket()
{
	CIOCPData *pIOCPKey;	
	list_head *pHead, *pEach, *pSafe;

	pHead = GetListAll();
	
	list_for_each_safe(pEach, pSafe, pHead)
	{
		pIOCPKey = list_entry_offset(pEach, CIOCPData, m_ListAllSocket);
		CloseSocket(pIOCPKey);
	}

	//accept的监听
	int nCount = m_LocalObjectAccept.size();
	for(int i=0; i<nCount; ++i)
	{
		closesocket(m_LocalObjectAccept[i]->GetSocket());
	}
}

CIOCPData* CSocketRun::MallocNetObject()
{
	void *p = g_KmemManager.KmemMalloc(sizeof(CIOCPData));	

	if( NULL==p )
	{
		return NULL;
	}

	CIOCPData *pIOCPData = new(p)CIOCPData;
	//printf("CSocketRun::MallocNetObject(), %x\n",pIOCPData );
	return pIOCPData;
}
void CSocketRun::FreeNetObject(CIOCPData* p)
{
	//根据C++对象原则，主动调用析构函数
	//printf("CSocketRun::FreeNetObject, %x\n", p);
	if(NULL==p)
	{
		return;
	}
	p->~CIOCPData();
	g_KmemManager.KmemFree((void*)p);
}

/*
	存取接口函数
*/
list_head* CSocketRun::GetListSend()
{
	return &m_ListSendActive.m_ListHead;
}
list_head* CSocketRun::GetListRecv()
{
	return &m_ListRecvActive.m_ListHead;
}
list_head* CSocketRun::GetListAll()
{
	return &m_ListAllSocket.m_ListHead;
}
bool CSocketRun::IsThreadRun()
{
	return true==m_bThreadRun;
}