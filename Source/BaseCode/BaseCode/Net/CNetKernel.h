/*
	@lindp lin_style@foxmail.com   
	@2012/1/29
	@��������ع������������߳�
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
		AddClientSocket,Init()˳��Ҫע�⣬AddClientSocketҪ��Init()֮ǰ����ʱ��֧�ֶ�̬���
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
	void AddClientSocket(CSocketClient* pSocketClient);
	void CloseClientSocket(CSocketClient* pSocketClient, bool bNotifyLogic=true);
	bool VerifySocketClientValid(unsigned int nSocketKey);
	CSocketClient* GetSocketClientByKey(unsigned int nSocketKey);
	/*
		help����
		OnAcceptSocket:�����׽��ֽ���
		OnRecvSocket:���׽����յ�����
	*/
	bool _LoopIOCP();
	void OnAcceptSocket(CSocketClient* pSocketClient);
	void OnRecvSocket(CSocketClient* pSocketClient);
public:

private:
	bool m_bHasInitSocket;
	CIOCP m_IOCP;

	//�߳�����
	HANDLE m_hThreadLoop;
	volatile bool m_bThreadRun;	
	unsigned int   m_uThreadLoop;
	
	//���������׽��֡�keyΪ���߼���֮��Ψһ��ʶ��Ϣ
	typedef map<unsigned int,CSocketClient *> MAP_SOCKETCLIENT;
	MAP_SOCKETCLIENT m_mapAllClientSocket;

	//��Ҫ�������׽���
	CList m_lstConnect;
};
