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
		LoopBridgeQueue: �Ӻ��߼��㽻���Ķ�����ȡ�������
	*/
	void Loop();
	void LoopIOCP();
	void LoopSendData();
	void LoopConnect();
	void LoopBridgeQueue();
	
	/*
		AddAcceptSocket
		���һ�����ӽ������׽��ֶ���
			
		CloseClientSocket
		�ر�һ���׽��ֶ������

		VerifySocketClientValid
		���CSocketClient�����Ƿ���Ч
	*/
	void AddClientSocket(CSocketClient *pSocketClient);
	void CloseClientSocket(CSocketClient *pSocketClient, bool bNotifyLogic=true);
	bool VerifySocketClientValid(unsigned int nSocketKey);
	CSocketClient* GetSocketClientByKey(unsigned int nSocketKey);
	/*
		help����
		OnAcceptSocket:�����׽��ֽ���
		OnRecvSocket:���׽����յ�����
	*/
	bool _LoopIOCP();
	void OnAcceptSocket(CSocketClient *pSocketClient);
	void OnRecvSocket(CSocketClient *pSocketClient);
public:

private:
	CIOCP m_IOCP;

	//�߳�����
	HANDLE m_hThreadLoop;
	volatile bool m_bThreadRun;	
	unsigned int   m_uThreadLoop;
	
	/*
		����SocketClient��hash�����߼��㴫�����ʱ�����������Ѿ�ɾ���������
		��Ҫ�ȼ���Ƿ���ڸ�ֵ
	*/
	typedef map<unsigned int,CSocketClient *> HASH_SOCKETCLIENT;
	HASH_SOCKETCLIENT m_HashSocketClient;

	//��Ҫ�������׽���
	CList m_lConnect;
	//�����������ӵ�����
	CList m_lSocketClient;
};
