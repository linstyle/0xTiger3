/*  作者：		林东平                                                              
	创建时间：	2010/07/20                                                        
    功能说明：  网络对象
*/
#pragma once
#include "CSocketAPI.h"
#include "IPackets.h"
#include "ListArrayMap.h"
#include "CLock.h"
#include "LinuxList.h"
#include "CSocketSendBuffer.h"
#include "CSocketRecvBuffer.h"

enum IOCP_EVENT_TYPE
{
	/*
		iocp用到的其实就CONNECT, BIG
	*/
	IOCP_EVENT_ACCEPT_CONNECT,
	IOCP_EVENT_RECV_HEAD,
	IOCP_EVENT_RECV_CONTENT,
	IOCP_EVENT_RECV_BIG,
	IOCP_DEBUG_NEW,
	IOCP_DEBUG_FREE
};

/*
	所属位置标识
*/
class CBelongToList
{
public:
	CBelongToList();
	
	void SetSendList(bool b);
	void SetRecvList(bool b);
	void SetIOCPPost(bool b);
	void SetAllSocketList(bool b);

	bool IsSendList();
	bool IsRecvList();
	bool IsIOCPPost();
	bool IsAllSocketList();

private:
	void Init();

public:

private:
	bool m_bSendList;
	bool m_bRecvList;
	bool m_bIOCPPost;	//投递IOCP中
	bool m_bAllSocket;
};


class CIOCPData:public CKeyId/*ListArrayMap*/
{
public:
	CIOCPData();
	~CIOCPData();

	void Init();
	/*
		Send()
		返回值:
				1: 没有全部发完
				0: 全部发完
			   -1: 出错
	*/	
	int Send(IPackHead *pPackHead);	//SendBufferWrite()
	int FlushSend();				//SendBufferFlushRead();
	
	/*
		接收的顺序:收取协议头，获取内容长度，接受协议内容
		RecvPacketHead
		InitRecvPacketContent
		RecvPacketContent

		Recv(); 物理上的读
		RecvPacketHead, RecvPacketContent;逻辑上的读（从缓冲区上）
	*/
	int Recv();
	int RecvPacketHead();
	int RecvPacketContent(unsigned int nLen);
	int InitRecvPacketContent();
	void FlushRecv(unsigned int nLen); //RecvBufferFlushWrite();

private:
	int FlushSendSub();
	int RecvPacket();

	void InitRecv();
	void InitRecv1();
	void InitRecv2();

public:
	/*
		完成端口需要的变量以及数据区
	*/
	SOCKET nSocket;
	DWORD nRecvBytes;
	DWORD nWSARecvFlag;
	WSABUF WSADataBuf[2];
	WSAOVERLAPPED  OverlappedRecv;

	/*
		BufferPacket:每个具体包的大小存放
	*/
	char BufferPacket[name_msg_packet::SOCKET_BUFF_SIZE];
	CSocketRecvBuffer RecvBuffer;
	CSocketSendBuffer SendBuffer;
	
	IOCP_EVENT_TYPE nEvent;

	CBelongToList BelongToList;

	struct list_head m_ListSend;
	struct list_head m_ListRecv;
	struct list_head m_ListAllSocket;
private:
};
