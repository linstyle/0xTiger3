#include "PPackets.h"
#include "MTASSERT.h"
#include "mystdio.h"


IPacketHead::IPacketHead()
{
	MEMSET(this, 0, sizeof(IPacketHead));
}

char* IPacketHead::GetPacketBuffer()
{
	return m_Buffer;
}

unsigned short IPacketHead::GetPacketSize()
{
	return m_PacketHead.m_nPacketSize;
}

unsigned short IPacketHead::GetPacketDefine1()
{
	return m_PacketHead.m_nPacketDefine1;
}

unsigned short IPacketHead::GetPacketDefine2()
{
	return m_PacketHead.m_nPacketDefine2;
}

/********************************
		IPacketObject
*********************************/
IPacketObject::IPacketObject(PACKET_DEFINE1 nPacketDefine)
{
	m_nPacketDefine = nPacketDefine;
}
PACKET_DEFINE1 IPacketObject::GetPacketDefine1()
{
	return m_nPacketDefine;
}