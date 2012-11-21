/*  作者：		林东平                                                              
	创建时间：	2010/06/21                                                         
    功能说明：  套接字api的常见操作
*/

#pragma once
#pragma comment(lib,"Ws2_32.lib")

#include <Winsock2.h>
#include <stdio.h>
#include "MTASSERT.h"

//#define printf(level) printf(level##",%s::%s:%d, Err:%d\n", __FILE__, __FUNCTION__, __LINE__, WSAGetLastError())

class CSocketAPI
{
public:

	/*
		函数功能: 初始化对应的协议端
	*/
	static bool InitSocketTCPS(SOCKET &Socket, ULONG nIP, USHORT nPort, ULONG nNonb=0);
	static bool InitSocketTCPC(SOCKET &Socket, ULONG nIP, USHORT nPort, ULONG nNonb=0);
	
	static bool InitSocketUDPS(SOCKET &Socket, ULONG nIP, USHORT nPort, ULONG nNonb=0);
	static bool InitSocketUDPC(SOCKET &Socket, ULONG nIP, USHORT nPort, ULONG nNonb=0);

	/*
		函数功能: 封装套接字的动作
		返回值:   -1 失败
				  >=0 成功，或者是接收/发送的字节数
	*/
	static int ConnectNonb(SOCKET fdNonb, const sockaddr *pConnectAddr, int nConnectAddrSize, int nSec);
	static int RecvnTCP(SOCKET fd, char* pRecvBuffer, int nRecvLen);
	static int SendnTCP(SOCKET fd, const char* pSendBuffer, int nSendLen);

	static int RecvnUDP(SOCKET fd, char* pRecvBuffer, int nRecvLen, int nRecvFlag, sockaddr* FromAddr);
	static int SendnUDP(SOCKET fd, const char* pSendBuffer, int nSendLen, int nSendFlag, const struct sockaddr* SendAddr);

	static int Close(SOCKET fd);
private:

public:



private:

};