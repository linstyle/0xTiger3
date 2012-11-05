#include "stdafx.h"
#include "NetAPI.h"


bool net::AddSocketServer(const char* pName, const char* pListenIP, USHORT nListenPort)
{
	return g_NetDriver2.AddSocketServer(pName, pListenIP, nListenPort);
}

bool net::AddClientSocket(const char* pConnectIP, USHORT nConnectPort)
{
	return g_NetDriver2.AddClientSocket(pConnectIP, nConnectPort);
}

int net::GetPacketStream(char *pBuffer, int nBufferLen)
{
	return g_NetDriver2.GetPacketStream(pBuffer, nBufferLen);
}

int net::PutPacketStream(const char *pBuffer, int nBufferLen)
{
	return g_NetDriver2.PutPacketStream(pBuffer, nBufferLen);
}


//bool net::NoticNetErr()
//{
//
//}