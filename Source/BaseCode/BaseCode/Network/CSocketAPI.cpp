#include "StdAfx.h"
#include "CSocketAPI.h"

bool CSocketAPI::InitSocketTCPS(SOCKET &Socket, ULONG nIP, USHORT nPort, ULONG nNonb)
{	
	int nResult;
	sockaddr_in Addr;

	Socket = WSASocket( AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED );
	if( INVALID_SOCKET==Socket )
	{
		return false;
	}

	ZeroMemory(&Addr, sizeof(Addr));
	Addr.sin_family	= AF_INET;
	Addr.sin_port	= htons(nPort);
	Addr.sin_addr.s_addr = nIP;

	nResult = bind(Socket, (sockaddr*)&Addr, sizeof(Addr));
	if( SOCKET_ERROR==nResult )
	{
		return false;
	}

	nResult = listen(Socket,3);
	if( SOCKET_ERROR==nResult )
	{
		return false;
	}

	nResult = ioctlsocket(Socket, FIONBIO, &nNonb);
	if( SOCKET_ERROR==nResult )
	{
		return false;
	}

	return true;
}

bool CSocketAPI::InitSocketTCPC(SOCKET &Socket, ULONG nIP, USHORT nPort, ULONG nNonb)
{
	// Prepare a socket to listen for connections.
	int nResult;
	sockaddr_in Addr;

	Socket = socket(AF_INET, SOCK_STREAM, 0);
	if( INVALID_SOCKET==Socket )
	{
		return false;
	}
	
	nResult = ioctlsocket(Socket, FIONBIO, &nNonb);
	if( SOCKET_ERROR==nResult )
	{
		return false;
	}

	ZeroMemory(&Addr, sizeof(Addr));
	Addr.sin_family	= AF_INET;
	Addr.sin_port	= htons(nPort);
	Addr.sin_addr.s_addr = nIP;

	return true;
}

bool CSocketAPI::InitSocketUDPS(SOCKET &Socket, ULONG nIP, USHORT nPort, ULONG nNonb)
{
	int nResult;
	sockaddr_in Addr;

	Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if( INVALID_SOCKET==Socket )
	{
		return false;
	}

	nResult = ioctlsocket(Socket, FIONBIO,(unsigned long*)&nNonb);
	if( SOCKET_ERROR==Socket )
	{
		return false;
	}

	ZeroMemory(&Addr, sizeof(Addr));
	Addr.sin_family	   = AF_INET;
	Addr.sin_port		= htons(nPort);
	Addr.sin_addr.s_addr = nIP;

	nResult = bind(Socket, (sockaddr*)&Addr, sizeof(Addr) );
	if( SOCKET_ERROR==nResult )
	{
		return false;
	}

	return true;
}

bool CSocketAPI::InitSocketUDPC(SOCKET &Socket, ULONG nIP, USHORT nPort, ULONG nNonb)
{
	int nResult;
	

	Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if( INVALID_SOCKET==Socket )
	{
		return false;
	}

	nResult = ioctlsocket(Socket, FIONBIO,(unsigned long*)&nNonb);
	if( SOCKET_ERROR==Socket )
	{
		return false;
	}
/*
	sockaddr_in Addr;
	ZeroMemory(&Addr, sizeof(Addr));
	Addr.sin_family	   = AF_INET;
	Addr.sin_port		   = htons(nPort);
	Addr.sin_addr.s_addr = nIP;
*/
	return true;
}

int CSocketAPI::ConnectNonb(int fdNonb, const sockaddr *pConnectAddr, int nConnectAddrSize, int nSec)
{
	int nResult;
	/*  
		windows下不支持对套接字的还原设定，所以掉用本函数后，自动变为非阻塞
	*/

	ULONG argp = 1;
	nResult = ioctlsocket(fdNonb, FIONBIO,(ULONG*)&argp);
	if( SOCKET_ERROR==nResult )
	{
		return -1;
	}

	nResult = connect(fdNonb, pConnectAddr, nConnectAddrSize);
	if( WSAEWOULDBLOCK != WSAGetLastError())
	{
		closesocket(fdNonb);
		return -1;
	}		
	else if(0==nResult)				//connect completed immediately 
	{
		return 0;
	}


	fd_set SetWrite;
	timeval TimeSelect;

	TimeSelect.tv_sec=nSec;
	TimeSelect.tv_usec=0;

	FD_ZERO(&SetWrite);
	FD_SET(fdNonb, &SetWrite);

	nResult = select(0, NULL, &SetWrite, NULL, &TimeSelect);
	if( nResult<=0 )
	{
		closesocket(fdNonb);
		WSASetLastError(WSAETIMEDOUT);
		return -1;
	}

	if( FD_ISSET(fdNonb, &SetWrite) )
	{
		return 0;
	}

	return -1;
}

int CSocketAPI::RecvnTCP(SOCKET fd, char* pRecvBuffer, int nRecvLen)
{
	char *Ptr = pRecvBuffer;
	int nRecv = nRecvLen;
	int nRecvHasLen;

	while( nRecv>0 )
	{
		nRecvHasLen = recv(fd, Ptr, nRecv, 0);
		if( nRecvHasLen<=0 )
		{
			if( WSAEWOULDBLOCK != WSAGetLastError())
			{
				return -1;
			}
			//这里根据实际业务要求，要接收几次。(默认是只一次),然后返回已接收的长度 nRecvLen-nRecv
			//would block , must read again
			return nRecvLen-nRecv;
		}

		nRecv -= nRecvHasLen;
		Ptr += nRecvHasLen;
	}

	return nRecvLen-nRecv;
}

int CSocketAPI::SendnTCP(SOCKET fd, const char* pSendBuffer, int nSendLen)
{
	const char *Ptr = pSendBuffer;
	int nSend = nSendLen;
	int nSendHasLen;

	while( nSend>0 )
	{
		nSendHasLen = send(fd, Ptr, nSend, 0);
		if( nSendHasLen<=0 )
		{
			if( WSAEWOULDBLOCK != WSAGetLastError())
			{
				return -1;
			}
			//
			//空间不足或其他原因，返回已发送的数			
			return nSendLen-nSend;
		}

		nSend -= nSendHasLen;
		Ptr += nSendHasLen;

	}

	return nSendLen;
}

int CSocketAPI::RecvnUDP(SOCKET fd, char* pRecvBuffer, int nRecvLen, int nRecvFlag, sockaddr* FromAddr)
{
	int nResult;
	int nRecvAddrSize = sizeof(*FromAddr);

	nResult = recvfrom(fd, pRecvBuffer, nRecvLen, nRecvFlag, FromAddr, &nRecvAddrSize);
	if( SOCKET_ERROR==nResult )
	{
		return -1;
	}

	return nResult;
}


int CSocketAPI::SendnUDP(SOCKET fd, const char* pSendBuffer, int nSendLen, int nSendFlag, const struct sockaddr* SendAddr)
{
	int nResult;
	int nSendAddrSize = sizeof(*SendAddr);
	
	nResult = sendto(fd, pSendBuffer, nSendLen, nSendFlag, SendAddr, nSendAddrSize);
	if( nResult<nSendLen )
	{
		//有错误发生或者发送的字节数不够
		return -1;
	}
	
	return nResult;
}