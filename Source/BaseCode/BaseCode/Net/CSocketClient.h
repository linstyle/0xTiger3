/*
	@lindp lin_style@foxmail.com   
	@2012/1/24
	@重写下网络代码之，网络客户端对象
*/
#pragma  once
#include <winsock2.h>
#include "LinuxList.h"
#include "CCircleBuffer.h"
#include "CLog2.0.h"

//#include "CWindowsSlabManager.h"
/*
	分配空间的全局API
*/
class CSocketClient;
CSocketClient* MallocSocketClientObject();
void FreeSocketClientObject(CSocketClient* p);

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


namespace socket_circle_config
{
	const int RECV_CIRCLE_LEN = 16*1024;  //kb, 系统默认套接字是8KB，两倍大小即可。
	const int SEND_CIRCLE_LEN = 16*1024; //kb，跟具体业务有关
};

static unsigned int SOCKET_KEY_SEED = 0;
class CSocketClient
{
public:
	CSocketClient();
	~CSocketClient();

	void Init();

	int Connect();
	bool Recv();
	bool Send(const char* pBuffer, int nBufferLen);
	int FlushSend();

	unsigned int GetKey();
private:
	/*
		@send,recv,connect函数
	*/
	bool InitConnect();
	int  FlushSendSub();

	void InitRecv();
	void InitRecv1();
	void InitRecv2();
	bool RecvPacketStream();

public:
	SOCKET m_nSocket;
	USHORT m_nPort;
	ULONG  m_nIP;	

	/*
		connect
	*/
	sockaddr_in m_ConnectAddr;
	bool m_bAutoConnect; //如果设为true,则不会从链表从删除，断开时会自动重连
	
	/*
		完成端口需要的变量以及数据区
	*/
	DWORD m_nRecvBytes;
	DWORD m_nWSARecvFlag;
	WSABUF m_WSADataBuf[2];
	WSAOVERLAPPED  m_OverlappedRecv;
	IOCP_EVENT_TYPE m_nIOCPEvent;
	CCircleBuffer m_RecvBuffer;
	CCircleBuffer m_SendBuffer;

	//传递给逻辑层的key,直接用地址的话，因为是异步的，怕重复
	unsigned int m_nKey;

	/*
		m_lConnectNode:  CNetKernel:m_lstConnect使用
		m_lConnectNode:  CNetKernel:m_lstAllSocketClient使用
	*/
	struct list_head m_lConnectNode;
	struct list_head m_lAllSocketClient;
private:

};

