/*
	@lindp lin_style@foxmail.com   
	@2012/1/29
	@��������ع������������߳�
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
		AddClientSocket,Init()˳��Ҫע�⣬AddClientSocketҪ��Init()֮ǰ����ʱ��֧�ֶ�̬���
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
		�⼸��API��ʾ�˺��ĵļ�������
		LoopIOCP����IOCP�л�ȡ���ݣ������������ӡ��������ݷ��ص�
		SendAllData: �����е����ݷ��ͳ�ȥ
		OnRecvData: �������ݣ��͵�����
		Connect: ���Ƿ�����Ҫ���ӵ��׽���
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
		���һ�����ӽ������׽��ֶ���
			
		CloseClientSocket
		�ر�һ���׽��ֶ������

		VerifySocketClientValid
		���CSocketClient�����Ƿ���Ч
	*/
	bool AddClientSocket(CSocketClient *pSocketClient);
	void CloseClientSocket(CSocketClient *pSocketClient);
	bool VerifySocketClientValid(CSocketClient *pSocketClient);

	/*
		help��
		OnAcceptSocket:�����׽��ֽ���
	*/
	void OnAcceptSocket(CSocketClient *pSocketClient);
	void OnRecvSocket(CSocketClient *pSocketClient);
public:

private:
	//�߳�����
	HANDLE m_hThreadLoop;
	volatile bool m_bThreadRun;	
	unsigned int   m_uThreadLoop;
	
	CIOCP m_IOCP;

	//�����������ӵ�����
	CList m_ListSocketClient;

	/*
		����SocketClient��hash�����߼��㴫�����ʱ�����������Ѿ�ɾ���������
		��Ҫ�ȼ���Ƿ���ڸ�ֵ
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
//		AddClientSocket,Init()˳��Ҫע�⣬AddClientSocketҪ��Init()֮ǰ����ʱ��֧�ֶ�̬���
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
//	//DoSocketClientErr,�߼���ص���֪ͨ������ҪNoticLogic
//	void DoSocketClientErr(CSocketClient *pSocketClient);
//	void DoSocketClientErrAndNoticLogic(CSocketClient *pSocketClient); 
//	void DoSocketClientRecv(CSocketClient *pSocketClient);
//	void DoSocketClientConnect(CSocketClient *pSocketClient);
//	void DoSocketClientAccept(CSocketClient *pSocketClient);
//	void DoBridgeQueue(const char *pBuffer);
//	void DoBridgeNLInnerNotic(PNLInnerNotic *pNLInnerNotic); //�����ڲ�(logic->net)Э��
//
//	//Ͷ��һ��WSARECV
//	void PostIOCPRecv(CSocketClient *pSocketClient);
//
//	//���CSocketClient�Ƿ���Ч
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
//	//�����������ӵ�����
//	CList m_ListSocketClient;
//
//	/*
//		����SocketClient��hash�����߼��㴫�����ʱ�����������Ѿ�ɾ���������
//		��Ҫ�ȼ���Ƿ���ڸ�ֵ
//	*/
//	typedef map<CSocketClient *,CSocketClient *> HASH_SOCKETCLIENT;
//	HASH_SOCKETCLIENT m_HashSocketClient;
//};