/*  
	作者：		林东平                                                              
	创建时间：	2010/02/14                                                         
    功能说明：  套接字的整个工作流程
*/

#pragma once

#include "GlobalType.h"
#include "CLock.h"
#include "CSlab.h"
#include "LinuxList.h"
#include "CNetObject.h"
#include "CNetTracer.h"
#include "ILocalObject.h"
#include "CNetTransToLogic.h"

class CSocketRun;
namespace name_socket_run
{
	struct THREAD_PARAM_ACCPET
	{
		CSocketRun *pSocketRun;
		ILocalObjectAccept *pObjectAccept;
	};
};

class CSocketRun
{
public:
	~CSocketRun();

	void Init(ILocalObjectConnect* pObjectConnect, int nConnectCount, 
			  ILocalObjectAccept* pObjectAccept, int nAcceptCount);
	void TracerMe();

	void Run();
	int  Send(CIOCPData *pIOCPKey, IPackHead *pPackHead);
	void CloseSocket(CIOCPData *pIOCPKey);

	/*
		存取接口函数
	*/
	//线程是否可以继续运行
	bool IsThreadRun();
private:
	/*
		初始化
	*/
	bool InitIOCP();
	bool InitConnect(ILocalObjectConnect* pObjectConnect, int nConnectCount);
	bool InitAccept(ILocalObjectAccept* pObjectAccept, int nAcceptCount);
	
	/*
		资源之间的具体操作
	*/
	void FlushRecv();
	void FlushSend();
	void FlushIOCP();
	bool _FlushIOCP();
	void FlushConnect();
	void _FlushConnect(int nIndex);

	//send
	int AddSend(CIOCPData *pIOCPKey);
	int DelSend(CIOCPData *pIOCPKey);
	//recv
	int AddRecv(CIOCPData *pIOCPKey);
	int DelRecv(CIOCPData *pIOCPKey);
	void ResetRecv(CIOCPData *pIOCPKey);	//重新接收
	//allsocket
	int AddAllSocket(CIOCPData *pIOCPKey);
	int DelAllSocket(CIOCPData *pIOCPKey);
	//iocp
	int AddIOCPPost(CIOCPData *pIOCPKey);
	int DelIOCPPost(CIOCPData *pIOCPKey);
	void IOCPPostConnect(CIOCPData *pIOCPKey);	//投递一个IOCP连接事件
	void IOCPPostRecv(CIOCPData *pIOCPKey);		//投递一个IOCP接收事件
	//connect
	int  AddLocalConnect(ILocalObjectConnect* pLocalObject);
	void DoAcceptConnect(CIOCPData *pIOCPKey);
	//accept
	void AddLocalAccept(ILocalObjectAccept* pLocalObject);
	static unsigned int WINAPI ProcAccept(void* param);
	bool StartThreadAccept();
	void EndThreadAccept();

	//执行上层包
	int ExcutePacket(CIOCPData* pIOCPKey);

	/*
		关闭
	*/
	//关闭套接字以及上层的逻辑对象
	//void CloseSocketAndLogic(CIOCPData *pIOCPKey);
	//关闭所有套接字资源
	void CloseAllSocket();
	//关闭本地套接字接口
	int  CloseLocalSocket(CIOCPData* pIOCPKey);

	/*
		对new和delete的资源封装
	*/
	CIOCPData* MallocNetObject();
	void FreeNetObject(CIOCPData* p);

	/*
		存取接口函数
	*/
	list_head* GetListSend();
	list_head* GetListRecv();
	list_head* GetListAll();
public:

private:
	CIOCP m_IOCP;
	/*
		m_ListSendActive:发送链表
		m_ListRecvActive:接收链表
		m_ListAllSocket:所有套接字的链表
	*/
	CList m_ListSendActive;
	CList m_ListRecvActive;
	CList m_ListAllSocket;

	std::vector<ILocalObjectConnect*> m_LocalObjectConnect;
	std::vector<ILocalObjectAccept*> m_LocalObjectAccept;
	std::vector<HANDLE> m_hThreadAccept;
	//控制线程是否执行
	bool m_bThreadRun;		
};
