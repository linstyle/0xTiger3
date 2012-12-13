/*
	@lindp lin_style@foxmail.com   
	@2012/1/29
	@��������ع��������߳�
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

	//���̺߳���ʹ��
	bool IOCPPostConnect(CSocketClient *pSocketClient);

	void Release();
	void ReleaseThread();

	void InitThread();	
	bool InitAccpetExlpfn(CSocketServer *pSocketServer);

public:

private:
	bool m_bHasInitSocket;
	//�߳�
	HANDLE m_hThreadAccept;
	volatile bool m_bThreadRun;	
	unsigned int m_uThreadAccept;

	CSocketServer m_SocketServer;

	//iocp,�ⲿ��ʼ��
	CIOCP *m_pIOCP;
};
