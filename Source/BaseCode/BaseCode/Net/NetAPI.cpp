
#include "NetAPI.h"


bool net::SetSocketServer(const char* pName, const char* pListenIP, USHORT nListenPort)
{
	return g_NetDriver2.SetSocketServer(pName, pListenIP, nListenPort);
}

bool net::AddConnectSocket(const char* pConnectIP, USHORT nConnectPort)
{
	return g_NetDriver2.AddClientSocket(pConnectIP, nConnectPort);
}

void net::Init()
{
	return g_NetDriver2.Init();
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