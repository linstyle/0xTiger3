/*
	@lindp lin_style@foxmail.com   
	@2012/1/29
	@网络代码重构，接收线程
*/
#include "CIOCP.h"
#include "CSocketServer.h"
#include "CSocketClient.h"


class CNetAccept
{
public:
	CNetAccept();
	~CNetAccept();
	
	void Init(CIOCP *pIOCP);
	bool CreateServerSocket(const char* pName, const char* pListenIP, USHORT nListenPort);
	CSocketServer* GetSocketServer();
	
private:
	static unsigned int WINAPI ThreadAccept(void* pParam);

	//给线程函数使用
	bool IOCPPostConnect(CSocketClient *pSocketClient);

	void Release();
	void ReleaseThread();

	void InitThread();	
	bool InitAccpetExlpfn(CSocketServer *pSocketServer);

public:

private:
	bool m_bHasInitSocket;
	//线程
	HANDLE m_hThreadAccept;
	volatile bool m_bThreadRun;	
	unsigned int m_uThreadAccept;

	CSocketServer m_SocketServer;

	//iocp,外部初始化
	CIOCP *m_pIOCP;
};
