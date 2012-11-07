/*
	@lindp lin_style@foxmail.com   
	@2012/1/24
	@重写下网络代码之，网络服务端对象
*/
#pragma once
#include "CSocketClient.h"
#include "CLog2.0.h"
#include <MSWSock.h>


class CSocketServer
{
public:
	CSocketServer();
	~CSocketServer();
	bool Accept(CSocketClient *pSocketClient);

private:
	void Init();
	bool InitAccept(CSocketClient *pSocketClient);

public:
	char m_szName[40];
	SOCKET m_nSocket;
	USHORT m_nPort;
	ULONG  m_nIP;

	//OS API需要
	WSAOVERLAPPED  m_OverlappedAccept;
	LPFN_ACCEPTEX m_lpfnAcceptEx;

private:

};

//
//class ILocalObjectAccept:public CSocketAPI
//{
//public:
//	virtual ~ILocalObjectAccept();
//
//	void Init(const char *name, ULONG nIP, USHORT nPort);
//	int Accept(CIOCPData *p);
//
//	SOCKET GetSocket();
//	ULONG GetIP();
//	USHORT GetPort();
//	const char* GetName();
//
//private:
//	bool InitAccept(CIOCPData *p);
//	void InitAccpetExlpfn();
//
//public:
//	WSAOVERLAPPED  m_OverlappedAccept;
//
//	LPFN_ACCEPTEX m_lpfnAcceptEx;
//
//private:	
//	char m_szName[40];
//	SOCKET m_nSocket;
//	USHORT m_nPort;
//	ULONG  m_nIP;
//
//	int  m_nAcceptExCount;
//};
