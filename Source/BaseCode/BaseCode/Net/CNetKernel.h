/*
	@lindp lin_style@foxmail.com   
	@2012/1/29
	@网络代码重构，核心运作线程
*/
#pragma once
#include "CIOCP.h"
#include "CSocketClient.h"
#include "CMyStackWalker.h"
#include "MTASSERT.h"
#include "PPackets.h"
#include <map>

using namespace std;

class CNetKernel
{
public:
	CNetKernel();
	~CNetKernel();

	/*
		AddClientSocket,Init()顺序要注意，AddClientSocket要在Init()之前，暂时不支持动态添加
	*/
	void Init();
	CIOCP* GetIOCP();

	bool CreateConnectSocket(const char* pConnectIP, USHORT nConnectPort, bool bAutoConnect = true);
	bool SendToBufferByNetKey(IPacketHead* pPacketHead, unsigned int nNetKey);
	void CloseClientSocketByNetKey(unsigned int nNetKey);
private:
	static unsigned int WINAPI ThreadLoop(void* pParam);

	void Release();

	void InitThread();
	void ReleaseThread();
	void ReleaseSocket();
 
	/*
		这几个API表示了核心的几个工作
		LoopIOCP：从IOCP中获取数据，比如有新连接、有新数据返回等
		SendAllData: 把所有的数据发送出去
		OnRecvData: 接收数据，送到队列
		Connect: 看是否有需要连接的套接字
		LoopBridgeQueue: 从和逻辑层交互的队列中取得任务包
	*/
	void Loop();
	void LoopIOCP();
	void LoopSendData();
	void LoopConnect();
	void LoopBridgeQueue();
	
	/*
		AddAcceptSocket
		添加一个连接进来的套接字对象
			
		CloseClientSocket
		关闭一个套接字对象对象

		VerifySocketClientValid
		检测CSocketClient对象是否有效
	*/
	void AddClientSocket(CSocketClient* pSocketClient);
	void CloseClientSocket(CSocketClient* pSocketClient, bool bNotifyLogic=true);
	bool VerifySocketClientValid(unsigned int nSocketKey);
	CSocketClient* GetSocketClientByKey(unsigned int nSocketKey);
	/*
		help函数
		OnAcceptSocket:当有套接字进来
		OnRecvSocket:当套接字收到数据
	*/
	bool _LoopIOCP();
	void OnAcceptSocket(CSocketClient* pSocketClient);
	void OnRecvSocket(CSocketClient* pSocketClient);
public:

private:
	bool m_bHasInitSocket;
	CIOCP m_IOCP;

	//线程数据
	HANDLE m_hThreadLoop;
	volatile bool m_bThreadRun;	
	unsigned int   m_uThreadLoop;
	
	//管理所有套接字。key为和逻辑层之间唯一标识信息
	typedef map<unsigned int,CSocketClient *> MAP_SOCKETCLIENT;
	MAP_SOCKETCLIENT m_mapAllClientSocket;

	//需要重连的套接字
	CList m_lstConnect;
};
