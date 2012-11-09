/*
	@lindp lin_style@foxmail.com   
	@2012/1/24
	@重写下网络代码
	重新定义下网络层：只是把数据从网络转成本地的形式，其他什么都不做。
	跟逻辑层的通信用队列保持，并且定义好事件枚举，即可。
	@更新 2012/2/19
	driver层调配着AcceptThread,KernelThread,NetCallBack三个组件，
	类的public也是对外开放的接口,对逻辑层统一暴露的API,其实就是封装了PNLInnerNotic协议。
	只是我觉得直接提供函数更友好点
*/

#pragma  once
#include "Singleton.h"

#include "CSocketClient.h"
#include "CSocketServer.h"
#include "CNetKernelThread.h"
#include "CNetAcceptThread.h"
#include "NetCallBackFun.h"
#include "PNLInnerNotic.h"
#include "CMyStackWalker.h"

enum
{
	NET_ENUM_CONNECT,
	NET_ENUM_DISCONNECT
};
/*
	线程； 接收线程，工作线程；
	连接对象管理；
	内存池管理；
	打包格式；
	写数据流队列，写事件队列；
*/
class CNetDriver2: public Singleton<CNetDriver2>
{
public:
	CNetDriver2();
	~CNetDriver2();

	void Init();
	/*
		@SetSocketServer
		@增加一个监听套接字,目前只支持一个

		@AddClientSocket
		@增加一个connect套接字	
	*/
	bool SetSocketServer(const char* pName, const char* pListenIP, USHORT nListenPort);	
	bool AddConnectSocket(const char* pConnectIP, USHORT nConnectPort);

	/*
		@NoticNetErr
		@通知网络层，逻辑层有错误发生
		@pNetObject:这个参数就是当网络层accept到一个套接字时，会通知逻辑层的附带参数
	*/
	bool NoticNetErr(void *pNetObject);

	/*
		@GetPacketStream
		@从队列中获取一个网络包
		@pNetObject:这个参数就是当网络层accept到一个套接字时，会通知逻辑层的附带参数
	*/
	int GetPacketStream(char *pBuffer, int nBufferLen);
	int PutPacketStream(const char *pBuffer, int nBufferLen);

private:
	/*
		初始化、释放
	*/
	void Release();
	void InitSocketLib();	
	void ReleaseSocketLib();
	void ReleaseAllConnect();

public:

private:
	CNetAcceptThread m_NetAcceptThread;
	CNetKernelThread m_NetKernelThread;

	bool m_bHasInit;

};

#define g_NetDriver2 CNetDriver2::getSingleton()