/*  
	作者：		林东平                                                              
	创建时间：	2010/11/3                                                        
    功能说明：  逻辑套接字，即被已经处于激活状态的套接字
*/
#pragma once

#include "GlobalType.h"
#include "CLock.h"
#include "CSlab.h"
#include "LinuxList.h"
#include "CNetObject.h"
#include "CNetTracer.h"
#include "CNetTransToLogic.h"

class CSocketLogic
{
public:
	CSocketLogic();
	~CSocketLogic();

	void TracerMe();

	/*
		注意，FlushException只是SocketPhysical中的一个子集
	*/
	void FlushRecv();
	void FlushSend();
	void FlushException(CIOCPData *pIOCPKey);
	void CloseAllSocket();				//关闭所有逻辑套接字

	int AddRecv(CIOCPData *pIOCPKey);
	int AddSend(CIOCPData *pIOCPKey);
	void AddException(CIOCPData *pIOCPKey, LONG nLastCount);
	
	list_head* GetListException();
private:
	/*
		DelSend,DelRecv只是从链表删除,然后挂载到Exception上
		DelException彻底回收资源
		Exception包含的意思有两种，一个是正常的断开，或者异常的数据错误等。这里都视为
				 底部抛了一个异常(非语法，理解上)上来，而导致的行为就是回收资源
	*/
	void DelSend(CIOCPData *pIOCPKey, bool bException=false);
	void DelRecv(CIOCPData *pIOCPKey, bool bException=false);
	void DelException(CIOCPData *pIOCPKey);
	void ResetActiveRecv(CIOCPData *pIOCPKey);

	int ExcutePacket(CIOCPData* pIOCPKey);
	int ClosePacket(CIOCPData* pIOCPKey);

	list_head* GetListSend();
	list_head* GetListRecv();
	
	
public:
	/*
		为了简化,m_ListSendActive实际上是所有套接字的一个链表(按线程分)。
		即使发送完该套接字数据后，也不会从链表上删除
	*/
	CList m_ListSendActive;
	CList m_ListRecvActive;
	CList m_ListExceptionActive;
	
private:

};

