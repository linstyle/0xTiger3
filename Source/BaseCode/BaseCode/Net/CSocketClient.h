/*
	@lindp lin_style@foxmail.com   
	@2012/1/24
	@��д���������֮������ͻ��˶���
*/
#pragma  once
#include <winsock2.h>
#include "LinuxList.h"
#include "CCircleBuffer.h"
#include "CLog2.0.h"

//#include "CWindowsSlabManager.h"
/*
	����ռ��ȫ��API
*/
class CSocketClient;
CSocketClient* MallocSocketClientObject();
void FreeSocketClientObject(CSocketClient* p);

enum IOCP_EVENT_TYPE
{
	/*
		iocp�õ�����ʵ��CONNECT, BIG
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
	const int RECV_CIRCLE_LEN = 16*1024;  //kb, ϵͳĬ���׽�����8KB��������С���ɡ�
	const int SEND_CIRCLE_LEN = 16*1024; //kb��������ҵ���й�
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
		@send,recv,connect����
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
	bool m_bAutoConnect; //�����Ϊtrue,�򲻻�������ɾ�����Ͽ�ʱ���Զ�����
	
	/*
		��ɶ˿���Ҫ�ı����Լ�������
	*/
	DWORD m_nRecvBytes;
	DWORD m_nWSARecvFlag;
	WSABUF m_WSADataBuf[2];
	WSAOVERLAPPED  m_OverlappedRecv;
	IOCP_EVENT_TYPE m_nIOCPEvent;
	CCircleBuffer m_RecvBuffer;
	CCircleBuffer m_SendBuffer;

	//���ݸ��߼����key,ֱ���õ�ַ�Ļ�����Ϊ���첽�ģ����ظ�
	unsigned int m_nKey;

	/*
		m_lConnectNode:  CNetKernel:m_lstConnectʹ��
		m_lConnectNode:  CNetKernel:m_lstAllSocketClientʹ��
	*/
	struct list_head m_lConnectNode;
	struct list_head m_lAllSocketClient;
private:

};

