#include "NetAPI.h"


bool net::CreateServerSocket(const char* pName, const char* pListenIP, USHORT nListenPort)
{
	return g_NetDriver2.CreateServerSocket(pName, pListenIP, nListenPort);
}

bool net::CreateConnectSocket(const char* pConnectIP, USHORT nConnectPort)
{
	return g_NetDriver2.CreateConnectSocket(pConnectIP, nConnectPort);
}

void net::Init()
{
	return g_NetDriver2.Init();
}


int net::SendPacket(IPacketHead* pPacketHead, CBasePlayer* pBasePlayer)
{
	return g_NetDriver2.SendPacket(pPacketHead, pBasePlayer);
}


bool net::CloseNet(CBasePlayer* pBasePlayer)
{
	return g_NetDriver2.CloseNet(pBasePlayer);
}