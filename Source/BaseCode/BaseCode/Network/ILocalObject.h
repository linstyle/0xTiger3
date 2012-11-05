/*  作者：		林东平                                                              
	创建时间：	2010/07/14                                                         
    功能说明：  本地网络对象
*/
#pragma once
#pragma comment(lib, "Mswsock.lib")

#include <assert.h>
#include "GlobalType.h"
#include "CNetObject.h"
#include "CIOCP.h"
#include <mswsock.h>

namespace name_netobject
{
	const int nAcceptIdenti=0;
	const int nConnectIdenti=1;
};

/*
	Connect(),Accept()
	这两个函数已经把底层的连接实现，用户在使用的时候可以根据自己需要重载这两个函数。
	比如进行一些额外的记录等等，然后返回基类的Connect/Accept()。

	返回值: -1失败
	0 成功
	1 已经处于状态中
*/
class ILocalObjectConnect:public CSocketAPI
{
public:
	void Init(const char *name, ULONG nIP, USHORT nPort);
	virtual ~ILocalObjectConnect();

	virtual int Connect();
	bool IsConnect();
	void ResetConnect();
	void SetAutoConnect(bool b);

	CIOCPData * GetIOCPData();
	SOCKET GetSocket();
	ULONG GetIP();
	USHORT GetPort();
	const char* GetName();

private:
	bool InitConnect();

public:


private:
	char m_szName[40];
	SOCKET m_nSocket;
	USHORT m_nPort;
	ULONG  m_nIP;	

	volatile bool m_bConnect;
	bool m_bAutoConnect;
	sockaddr_in m_ConnectAddr;
	CIOCPData m_IOCPData;
};


class ILocalObjectAccept:public CSocketAPI
{
public:
	virtual ~ILocalObjectAccept();

	void Init(const char *name, ULONG nIP, USHORT nPort);
	int Accept(CIOCPData *p);

	SOCKET GetSocket();
	ULONG GetIP();
	USHORT GetPort();
	const char* GetName();

private:
	bool InitAccept(CIOCPData *p);
	void InitAccpetExlpfn();
	
public:
	WSAOVERLAPPED  m_OverlappedAccept;

	LPFN_ACCEPTEX m_lpfnAcceptEx;

private:	
	char m_szName[40];
	SOCKET m_nSocket;
	USHORT m_nPort;
	ULONG  m_nIP;

	int  m_nAcceptExCount;
};
