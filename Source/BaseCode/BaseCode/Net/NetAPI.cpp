#include "NetAPI.h"


bool net::SetSocketServer(const char* pName, const char* pListenIP, USHORT nListenPort)
{
	return g_NetDriver2.SetSocketServer(pName, pListenIP, nListenPort);
}

bool net::AddConnectSocket(const char* pConnectIP, USHORT nConnectPort)
{
	return g_NetDriver2.AddConnectSocket(pConnectIP, nConnectPort);
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