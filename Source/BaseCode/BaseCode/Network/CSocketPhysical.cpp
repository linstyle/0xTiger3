#include "StdAfx.h"
#include "CSocketPhysical.h"
#include <process.h>


CSocketPhysical::CSocketPhysical()
{
	m_bThreadRun = true;
}

CSocketPhysical::~CSocketPhysical()
{
	/*
		等待connect,accept线程返回
	*/	
	CloseAllSocket();
	m_bThreadRun = false;
	
	EndThreadAccept();
	EndThreadConnect();
}

void CSocketPhysical::TracerMe()
{
	g_NetTracer.Tracer();
	g_KmemManager.TracerMe();
	//g_SocketActive.TracerMe();

	//int i;
	//int nCount = m_LocalObjectAccept.size();
	//for(i=0; i<nCount; ++i)
	//{
	//	printf("Accept Object:\n");
	//	printf(	"name:%s, (Accept port:%d)\n", m_LocalObjectAccept[i]->m_szName, m_LocalObjectAccept[i]->m_nPort);
	//}

	//for(i=0; i<nCount; ++i)
	//{
	//	in_addr in;
	//	in.S_un.S_addr = m_LocalObjectConnect[i]->m_nIP;

	//	printf("Connect Object:\n");
	//	printf(	"name:%s, %d (IP:%s, Port:%d)\n", m_LocalObjectConnect[i]->m_szName, m_LocalObjectConnect[i]->m_bConnect, 
	//		inet_ntoa( in ), m_LocalObjectConnect[i]->m_nPort);
	//}
}

void CSocketPhysical::Run()
{
	RunStatus();
	m_SocketLogic.FlushRecv();
	m_SocketLogic.FlushSend();
	/*
		FlushException除了逻辑套接字，也要物理套接字的操作，所以封装了一个，
		内部也是先调用逻辑的FlushException();
	*/
	FlushException();
}

void CSocketPhysical::Fire()
{
	StartIOCP();
	StartThreadConnect();
	StartThreadAccept();
}

void CSocketPhysical::AddLocalObjectConnect(ILocalObjectConnect* pLocalObject)
{
	m_LocalObjectConnect.push_back(pLocalObject);;	
}

void CSocketPhysical::AddLocalObjectAccept(ILocalObjectAccept* pLocalObject)
{	
	m_LocalObjectAccept.push_back(pLocalObject);
	pLocalObject->InitSocketTCPS(pLocalObject->m_nSocket, pLocalObject->m_nIP, pLocalObject->m_nPort);
	pLocalObject->InitAccpetExlpfn();
}

bool CSocketPhysical::StartIOCP()
{
	int nResult;

	nResult = m_IOCP.Create();	
	MTVERIFY( 1==nResult );

	return true;
}
 
bool CSocketPhysical::StartThreadAccept()
{
	unsigned int tid;	
	HANDLE handle;
	name_socket_physical::THREAD_PARAM_ACCPET *pTAcceptParam;
	
	for(int i=0; i<m_LocalObjectAccept.size(); ++i)
	{
		pTAcceptParam = new name_socket_physical::THREAD_PARAM_ACCPET;
		pTAcceptParam->pRun = &m_bThreadRun;
		pTAcceptParam->pObjectAccept = m_LocalObjectAccept[i];
		pTAcceptParam->pSocketPhysical = this;

		handle = (HANDLE)_beginthreadex(NULL, 0, ProcAccept,(void *)pTAcceptParam, 0, &tid);		
		MTVERIFY( 0!=handle );
		m_hThreadAccept.push_back(handle);
	}	
	return true;
}

bool CSocketPhysical::StartThreadConnect()
{
	unsigned int tid;
	HANDLE handle;
    
	if(m_LocalObjectConnect.size()<=0)
	{
		return true;
	}

	name_socket_physical::THREAD_PARAM_CONNECT *pTConnectParam;

	pTConnectParam = new name_socket_physical::THREAD_PARAM_CONNECT;
	pTConnectParam->pRun = &m_bThreadRun;
	pTConnectParam->pSocketPhysical = this;

	handle = (HANDLE)_beginthreadex(NULL, 0, ProcConnect,(void *) pTConnectParam, 0, &tid);
	MTVERIFY( 0!=handle );
	m_hThreadConnect.push_back(handle);

	return true;
}

void CSocketPhysical::EndThreadAccept()
{
	int nCount = m_hThreadAccept.size();
	for(int i=0; i<nCount; ++i)
	{
		if( WAIT_FAILED==WaitForSingleObject(m_hThreadAccept[i], INFINITE) )
		{
			prints("Err");
		}
	}
}

void CSocketPhysical::EndThreadConnect()
{
	int nCount = m_hThreadConnect.size();
	for(int i=0; i<nCount; ++i)
	{
		if( WAIT_FAILED==WaitForSingleObject(m_hThreadConnect[i], 3000) )
		{
			prints("Err");
		}
	}
}

void CSocketPhysical::FlushException()
{
	CIOCPData *pIOCPData;
	list_head *pHead, *pEach, *pSafe;

	pHead = m_SocketLogic.GetListException();
	
	list_for_each_safe(pEach, pSafe, pHead)
	{
		pIOCPData = list_entry_offset(pEach, CIOCPData, m_ListException);
		/*
			debug, 防止重复
		*/
		assert(true==pIOCPData->SocketGC.IsException());

		/*
			先逻辑本身的异常进行处理
		*/
		m_SocketLogic.FlushException(pIOCPData);
		CloseSocket(pIOCPData);		
		printf("Notic, CSocketPhysical::FlushException:%x\n", pIOCPData);
	}	
}

void CSocketPhysical::RunStatus()
{
	bool bResult=true;
	while( true==bResult)
	{
		bResult = FramStatus();
	}	
}

bool CSocketPhysical::FramStatus()
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
	if( 0==dwNumBytes || (false==bResult && NULL!=pOverLapped) )
	{
		if( IOCP_EVENT_ACCEPT_CONNECT!=pIOCPKey->nEvent )
		{
			printf("Notic, CSocketPhysical::FramStatus:GetClose:%x\n", pIOCPKey);				
			DelIOCPRecv(pIOCPKey, true);
			return true;
		}
	}

	/*
		FramStatus相当于IOCP的FlushRecv,所以这里判断是否发生异常
	*/
	if( true==pIOCPKey->SocketGC.IsException() )
	{
		DelIOCPRecv(pIOCPKey, true);
		return true;
	}

	/*
		表示已从投递链上收回
	*/
	DelIOCPRecv(pIOCPKey);
	
	switch (pIOCPKey->nEvent)
	{
	case IOCP_EVENT_ACCEPT_CONNECT:
		//printf("IOCP_EVENT_ACCEPT_CONNECT\n");
		AcceptConnect(pIOCPKey);
		break;

	case IOCP_EVENT_RECV_BIG:
		//printf("IOCP_EVENT_RECV_BIG:%d\n", dwNumBytes);	
		pIOCPKey->FlushRecv(dwNumBytes);
		if( -1==m_SocketLogic.AddRecv(pIOCPKey) )
		{
			printf("Warning, CSocketPhysical::FramStatus:AddRecv Failed\n");
		}
		
		break;

	default:
		printf("Warning, switch (pIOCPKey->nEvent) deault,nEvent:%d\n", pIOCPKey->nEvent);
	}

	return true;
}

void CSocketPhysical::AcceptConnect(CIOCPData* pIOCPKey)
{
	/*
		连接计数，这里还没有开始参与计数器，所以不需要socketGC的参与
	*/
	++g_NetTracer.m_nAccept;
	printf("Notic,accept ok,%d\n", pIOCPKey->nSocket);	

	if(false==pIOCPKey->SocketDebugFlag.IsActiveSend())
	{
		m_SocketLogic.AddSend(pIOCPKey);
	}

	if( -1==g_NetTransToLogic.AcceptConnect(pIOCPKey) )
	{
		CloseSocket(pIOCPKey);
		return ;
	}
	if( 0==m_IOCP.AssociateSocket(pIOCPKey->nSocket, (ULONG_PTR)pIOCPKey) )
	{
		printf("Err, CSocketPhysical::AcceptConnect, SocketNum:%d, ErrNum:%d\n", pIOCPKey->nSocket, WSAGetLastError());
		CloseSocket(pIOCPKey);
		return;
	}

	if( -1==pIOCPKey->Recv() )
	{
		CloseSocket(pIOCPKey);
	}
}



int CSocketPhysical::AddIOCPRecv(CIOCPData* pIOCPKey)
{
	/*
		这里只会被Accept,Connect调用。而WSARecv会自动Post到完成端口
	*/
	pIOCPKey->SocketGC.Increase();
	if( FALSE==m_IOCP.PostStatus((ULONG_PTR)pIOCPKey) )
	{
		printf("Err, CSocketPhysical::AddIOCPRecv:PostStatus,ErrNum:%d\n", WSAGetLastError());
		DelIOCPRecv(pIOCPKey, true);

		return -1;
	}

	return 0;
}

void CSocketPhysical::DelIOCPRecv(CIOCPData* pIOCPKey, bool bException)
{	
	if( true==bException )
	{
		m_SocketLogic.AddException(pIOCPKey, pIOCPKey->SocketGC.Decrease());	
	}
	else
	{
		pIOCPKey->SocketGC.Decrease();
	}
}

void CSocketPhysical::CloseSocket(CIOCPData* pIOCPKey)
{		
	/*
		关闭连接计数
	*/
	++g_NetTracer.m_nConnectDis;
	printf("Err, CSocketPhysical::Close Socket:%d ,ErrNum:%d\n", pIOCPKey->nSocket, WSAGetLastError());

	/*
		本地连接
	*/
	if( 0==CloseLocalObject(pIOCPKey) )
	{
		return;
	}

	closesocket(pIOCPKey->nSocket);
	FreeNetObject(pIOCPKey);
}

int CSocketPhysical::CloseLocalObject(CIOCPData* pIOCPKey)
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

void CSocketPhysical::CloseAllSocket()
{
	/*
		这里不用担心多线程的问题
		如果是Run归一个线程管，自然不用考虑
		如果Run是另一个线程的时间片，比如这里的NetDriver，势必等到NetDriver资源释放完后才执行这里
	*/
	//逻辑上的关闭
	m_SocketLogic.CloseAllSocket();
	
	/*
		accept,connect的本地关闭
	*/
	int i, nCount;
	nCount = m_LocalObjectAccept.size();
	for(i=0; i<nCount; ++i)
	{
		closesocket(m_LocalObjectAccept[i]->m_nSocket);
	}

	//nCount = m_LocalObjectConnect.size();
	//for(i=0; i<nCount; ++i)
	//{
	//	if( false==m_LocalObjectConnect[i]->IsConnect() )
	//	{
	//		//说明不在逻辑套接字中
	//		closesocket(m_LocalObjectConnect[i]->m_nSocket);
	//	}
	//}

	FlushException();
}

CIOCPData* CSocketPhysical::MallocNetObject()
{
	void *p = g_KmemManager.KmemMalloc(sizeof(CIOCPData));	

	if( NULL==p )
	{
		return NULL;
	}

	CIOCPData *pIOCPData = new(p)CIOCPData;
	//printf("CSocketPhysical::MallocNetObject(), %x\n",pIOCPData );
	return pIOCPData;
}

void CSocketPhysical::FreeNetObject(CIOCPData* p)
{
	/*
		根据C++对象原则，主动调用析构函数
	*/
	//printf("CSocketPhysical::FreeNetObject, %x\n", p);
	if(NULL==p)
	{
		return;
	}
	p->~CIOCPData();
	g_KmemManager.KmemFree((void*)p);
}


unsigned int WINAPI CSocketPhysical::ProcAccept(void* param)
{
	name_socket_physical::THREAD_PARAM_ACCPET *pTAcceptParam = (name_socket_physical::THREAD_PARAM_ACCPET *)param;
	CSocketPhysical *pSocketPhysical = pTAcceptParam->pSocketPhysical;
	ILocalObjectAccept *pObjectAccept = pTAcceptParam->pObjectAccept;

	int nResult;
	CIOCPData *p = NULL;
	while(true==*pTAcceptParam->pRun)
	{
		if( NULL==p )
		{
			p = pSocketPhysical->MallocNetObject();
			printf("Notic, CSocketPhysical::ProcAccept:Malloc %x:\n", p);
			if( NULL==p )
			{
				Sleep(1000);
				continue;
			}		
		}

		nResult = pObjectAccept->Accept(p);
		if( -1==nResult)
		{			
			/*
				出错计数
			*/
			++g_NetTracer.m_nAcceptErr;
			continue;
		}
		if(false==*pTAcceptParam->pRun)
		{
			//头疼这里的判断
			break;
		}

		/*
			如果投递失败，则意味着该连接即使成功，也无法正常工作。
			判定为失败。看成关闭该对象一类。
		*/
		pSocketPhysical->AddIOCPRecv(p);

		p = NULL;		
	}

	delete param;
	pSocketPhysical->FreeNetObject(p);
	printf("Notic, CSocketPhysical::ProcAccept:Free %x:\n", p);
	return 0;
}

unsigned int WINAPI CSocketPhysical::ProcConnect(void* param)
{
	name_socket_physical::THREAD_PARAM_CONNECT *pTConnectParam = (name_socket_physical::THREAD_PARAM_CONNECT *)param;
	CSocketPhysical *pSocketPhysical = pTConnectParam->pSocketPhysical;
	
	CIOCPData *p = NULL;
	while(true==*pTConnectParam->pRun)
	{				
		int nCount = pSocketPhysical->m_LocalObjectConnect.size();
		for(int i=0; i<nCount && true==*pTConnectParam->pRun; ++i)
		{
			ILocalObjectConnect* pLocalConnect =  pSocketPhysical->m_LocalObjectConnect[i];
			p = pLocalConnect->GetIOCPData();

			int nResult = pLocalConnect->Connect();
			if( -1==nResult )
			{
				/*
					出错计数
				*/
				++g_NetTracer.m_nConnectErr;
				printf("%s:\n", pLocalConnect->m_szName);
				prints("Err");
				continue;
			}
			else if( 1==nResult)
			{
				//已经是连接状态
				continue;
			}
			else
			{
				printf("Notic, Connect OK:%d\n", pLocalConnect->m_nSocket);

				p->nSocket = pLocalConnect->m_nSocket;				
				/*
					如果投递失败，则意味着该连接即使成功，也无法正常工作。
					判定为失败。看成关闭该对象一类。
				*/
				pSocketPhysical->AddIOCPRecv(p);
			}
		}

		Sleep(1000);
	}

	delete param;
	return 0;
}

