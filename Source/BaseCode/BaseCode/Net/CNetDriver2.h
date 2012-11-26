/*
	@lindp lin_style@foxmail.com   
	@2012/1/24
	@��д���������
	���¶���������㣺ֻ�ǰ����ݴ�����ת�ɱ��ص���ʽ������ʲô��������
	���߼����ͨ���ö��б��֣����Ҷ�����¼�ö�٣����ɡ�
	@���� 2012/2/19
	driver�������AcceptThread,KernelThread,NetCallBack���������
	���publicҲ�Ƕ��⿪�ŵĽӿ�,���߼���ͳһ��¶��API,��ʵ���Ƿ�װ��PNLInnerNoticЭ�顣
	ֻ���Ҿ���ֱ���ṩ�������Ѻõ�
	@ע�� 2012/11/20
	driver���Ǹ��߼�����õġ���driver������������·��ͨѶ����Ҫͨ������Ϣ����ʽ������
	�ڳ�ʼ��ʱ����SetSocketServer��AddConnectSocket����������
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
		@����һ�������׽���,Ŀǰֻ֧��һ��

		@AddClientSocket
		@����һ��connect�׽���	
	*/
	bool SetSocketServer(const char* pName, const char* pListenIP, USHORT nListenPort);	
	bool AddConnectSocket(const char* pConnectIP, USHORT nConnectPort);

	/*
		@CloseNet
		@֪ͨ����㣬�߼����д�����
		@nNetKey:����㴫�ݸ��߼�᯵ı�ʶ
	*/
	bool CloseNet(unsigned int nNetKey);

	//int GetPacketStream(char *pBuffer, int nBufferLen);
	bool SendPacket(IPacketHead* pPacketHead);

	CNetKernel* GetNetKerner();

private:
	/*
		��ʼ�����ͷ�
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