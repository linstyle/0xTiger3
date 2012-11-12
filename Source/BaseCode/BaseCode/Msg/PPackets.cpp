#include "PPackets.h"
#include "MTASSERT.h"
IPackHead::IPackHead(PACKET_DEFINE1 nPacketDefine1, int nPacketSize)
{		
	m_nRoutePriority = 0;
	m_nPacketDefine1 = nPacketDefine1;
	m_nPacketDefine2 = PACKET2_MIN;//用户手动设置，默认为MIN
	m_nPacketSize = nPacketSize;
	m_nNetKey = 0;
}

void IPackHead::SetNetKey(unsigned int nNetKey)
{
	m_nNetKey = nNetKey;
}
unsigned int  IPackHead::GetNetKey()
{
	return m_nNetKey;
}

void IPackHead::SetPacketDefine2(PACKET_DEFINE2 nPacketDefine2)
{
	m_nPacketDefine2 = nPacketDefine2;
}
void IPackHead::SetPacketSize(int nPacketSize)
{
	m_nPacketSize = nPacketSize;
}
PACKET_DEFINE1 IPackHead::GetPacketDefine1()
{
	return m_nPacketDefine1;
}
PACKET_DEFINE2 IPackHead::GetPacketDefine2()
{
	return m_nPacketDefine2;
}


//包大小，head+content
int IPackHead::GetPacketSize()
{
	return m_nPacketSize;
}

int IPackHead::GetPacketContentSize()
{
	return m_nPacketSize-sizeof(IPackHead);
}

/********************************
		IPacketObject
*********************************/

IPacketObject::IPacketObject(PACKET_DEFINE1 nPacketDefine)
{
	m_nPacketDefine = nPacketDefine;
}
PACKET_DEFINE1 IPacketObject::GetPacketID()
{
	return m_nPacketDefine;
}