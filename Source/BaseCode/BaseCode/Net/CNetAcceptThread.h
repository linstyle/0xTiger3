/*
	@lindp lin_style@foxmail.com   
	@2012/1/29
	@网络代码重构，接收线程
*/
#include "CIOCP.h"
#include "CSocketServer.h"
#include "CSocketClient.h"


class CNetAcceptThread
{
public:
	CNetAcceptThread();
	~CNetAcceptThread();
	
	/*
		AddSocketServer调用顺序不一定要在Init之前，也可以动态的调用。
		内部实现是一个数组，程序会先存储好值，然后改变数组下标，以供另外一个数组读取。
	*/
	void Init(CIOCP *pIOCP);
	bool AddSocketServer(const char* pName, const char* pListenIP, USHORT nListenPort);

	bool IOCPPostConnect(CSocketClient *pSocketClient);

	CSocketServer* GetSocketServerByIndex(int nIndex);
	
private:
	static unsigned int WINAPI ThreadAccept(void* pParam);

	void Release();

	void InitThread();	
	void ReleaseThread();
	
	bool InitSocketTCPServer(CSocketServer *pSocketServer);
	bool InitAccpetExlpfn(CSocketServer *pSocketServer);

public:

private:
	//线程
	volatile bool m_bThreadRun;
	HANDLE m_hThreadAccept;
	unsigned int   m_uThreadAccept;

	CSocketServer m_SocketServer[SOCKET_SERVER_COUNT];
	volatile int m_nSocketServerCount;

	//iocp,外部初始化
	CIOCP *m_pIOCP;
};
