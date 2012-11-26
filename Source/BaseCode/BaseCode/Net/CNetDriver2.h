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
	@注意 2012/11/20
	driver层是给逻辑层调用的。即driver层中如果想和网路层通讯，都要通过发消息的形式。除了
	在初始化时调用SetSocketServer，AddConnectSocket这两个函数
*/

#pragma  once
#include "Singleton.h"
#include "CSocketClient.h"
#include "CSocketServer.h"
#include "CNetKernel.h"
#include "CNetAccept.h"
#include "CNLBridgeQueue.h"

enum
{
	NET_ENUM_CONNECT,
	NET_ENUM_DISCONNECT
};

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
		@CloseNet
		@通知网络层，逻辑层有错误发生
		@nNetKey:网络层传递给逻辑岑的标识
	*/
	bool CloseNet(unsigned int nNetKey);

	//int GetPacketStream(char *pBuffer, int nBufferLen);
	bool SendPacket(IPacketHead* pPacketHead);

	CNetKernel* GetNetKerner();

private:
	/*
		初始化、释放
	*/
	void Release();
	void ReleaseAllConnect();

public:

private:
	CNetAccept m_NetAccept;
	CNetKernel m_NetKernel;

	bool m_bHasInit;

};

#define g_NetDriver2 CNetDriver2::getSingleton()