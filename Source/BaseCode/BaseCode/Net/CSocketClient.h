/*
	@lindp lin_style@foxmail.com   
	@2012/1/24
	@重写下网络代码之，网络客户端对象
*/
#pragma  once
#include <winsock2.h>
#include "LinuxList.h"
#include "CWindowsSlabManager.h"
#include "CCircleBuffer.h"
#include "CLog2.0.h"
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

enum IOCP_STEP_FLAG
{
	IOCP_STEP_ERR,
	IOCP_STEP_INIT,  //初始化状态
	IOCP_STEP_RECV_ING,
	IOCP_STEP_RECV_COMPLATE,
	IOCP_STEP_CONNECT,
};

namespace socket_circle_config
{
	const int RECV_CIRCLE_LEN = 16*1024;  //kb, 系统默认套接字是8KB，两倍大小即可。
	const int SEND_CIRCLE_LEN = 16*1024; //kb，跟具体业务有关
};


class CSocketClient
{
public:
	CSocketClient();
	~CSocketClient();

	void Init();

	int Connect();
	int Recv();
	int Send(const char* pBuffer, int nBufferLen);
	int FlushSend();

private:
	/*
		@send,recv,connect函数
	*/
	bool InitConnect();
	int FlushSendSub();
	int RecvPacketStream();
	void InitRecv();
	void InitRecv1();
	void InitRecv2();

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
	/*
		表示当前需要处理的队列
		-1:表示出错
		0:在recv投递中
		1:投递完成
		2:需要重连
	*/
	IOCP_STEP_FLAG m_nStepFlag;

	struct list_head m_List;
private:

};

