/*
	@lindp lin_style@foxmail.com   
	@2012/1/29
	@网络代码重构，核心运作线程
*/

#include "CIOCP.h"
#include "CSocketClient.h"
#include "PNLInnerNotic.h"
#include "CMyStackWalker.h"
#include "MTASSERT.h"
#include <map>
#include <vector>

using namespace std;

class CNetKernelThread
{
public:
	CNetKernelThread();
	~CNetKernelThread();

	/*
		AddClientSocket,Init()顺序要注意，AddClientSocket要在Init()之前，暂时不支持动态添加
	*/
	void Init();
	CIOCP* GetIOCP();

	bool AddConnectSocket(const char* pConnectIP, USHORT nConnectPort, bool bAutoConnect = true);

private:
	static unsigned int WINAPI ThreadLoop(void* pParam);

	void Release();

	void InitThread();
	void ReleaseThread();
 
	/*
		这几个API表示了核心的几个工作
		LoopIOCP：从IOCP中获取数据，比如有新连接、有新数据返回等
		SendAllData: 把所有的数据发送出去
		OnRecvData: 接收数据，送到队列
		Connect: 看是否有需要连接的套接字
	*/
	void Loop();
	void LoopIOCP();
	void LoopRecvData();
	void LoopSendData();
	void LoopConnect();
	
	
	bool _LoopIOCP();
	void OnRecvData(CSocketClient *pSocketClient);
	
	
	/*
		AddAcceptSocket
		添加一个连接进来的套接字对象
			
		CloseClientSocket
		关闭一个套接字对象对象

		VerifySocketClientValid
		检测CSocketClient对象是否有效
	*/
	bool AddClientSocket(CSocketClient *pSocketClient);
	void CloseClientSocket(CSocketClient *pSocketClient);
	bool VerifySocketClientValid(CSocketClient *pSocketClient);

	/*
		help类
		OnAcceptSocket:当有套接字进来
	*/
	void OnAcceptSocket(CSocketClient *pSocketClient);
	void OnRecvSocket(CSocketClient *pSocketClient);
public:

private:
	//线程数据
	HANDLE m_hThreadLoop;
	volatile bool m_bThreadRun;	
	unsigned int   m_uThreadLoop;
	
	CIOCP m_IOCP;

	//管理所有连接的链表
	CList m_ListSocketClient;

	/*
		现在SocketClient的hash表，当逻辑层传错误包时，网络层可能已经删除这个对象，
		故要先检测是否存在该值
	*/
	typedef map<unsigned int,CSocketClient *> HASH_SOCKETCLIENT;
	HASH_SOCKETCLIENT m_HashSocketClient;

	vector<CSocketClient*> m_vConnect;
};

//class CNetKernelThread
//{
//public:
//	CNetKernelThread();
//	~CNetKernelThread();
//
//	/*
//		AddClientSocket,Init()顺序要注意，AddClientSocket要在Init()之前，暂时不支持动态添加
//	*/
//	void Init();
//
//	bool AddClientSocket(const char* pConnectIP, USHORT nConnectPort, bool bAutoConnect = true);
//
//	CIOCP* GetIOCP();
//
//private:
//	static unsigned int WINAPI ThreadLoop(void* pParam);
//
//	void Release();
//
//	void InitThread();
//	void ReleaseThread();
//	
//	void Loop();
//	void LoopIOCP();
//	bool _LoopIOCP();
//	void LoopListSocketClient();
//	void LoopBridgeQueue();
//
//	void AddToClientSocketList(CSocketClient *pSocketClient);
//	void DelFromClientSocketList(CSocketClient *pSocketClient);
//
//	//DoSocketClientErr,逻辑层回调的通知，不需要NoticLogic
//	void DoSocketClientErr(CSocketClient *pSocketClient);
//	void DoSocketClientErrAndNoticLogic(CSocketClient *pSocketClient); 
//	void DoSocketClientRecv(CSocketClient *pSocketClient);
//	void DoSocketClientConnect(CSocketClient *pSocketClient);
//	void DoSocketClientAccept(CSocketClient *pSocketClient);
//	void DoBridgeQueue(const char *pBuffer);
//	void DoBridgeNLInnerNotic(PNLInnerNotic *pNLInnerNotic); //处理内部(logic->net)协议
//
//	//投递一个WSARECV
//	void PostIOCPRecv(CSocketClient *pSocketClient);
//
//	//检测CSocketClient是否有效
//	bool VerifySocketClientValid(CSocketClient *pSocketClient);
//public:
//
//private:
//	HANDLE m_hThreadLoop;
//	volatile bool m_bThreadRun;	
//	unsigned int   m_uThreadLoop;
//	
//	CIOCP m_IOCP;
//
//	//管理所有连接的链表
//	CList m_ListSocketClient;
//
//	/*
//		现在SocketClient的hash表，当逻辑层传错误包时，网络层可能已经删除这个对象，
//		故要先检测是否存在该值
//	*/
//	typedef map<CSocketClient *,CSocketClient *> HASH_SOCKETCLIENT;
//	HASH_SOCKETCLIENT m_HashSocketClient;
//};