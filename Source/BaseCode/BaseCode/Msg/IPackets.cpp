
#include "IPackets.h"
#include "MTASSERT.h"
IPackHead::IPackHead(PACKET_DEFINE1 nPacketDefine1, int nPacketSize)
{		
	m_nRoutePriority = 0;
	m_nPacketDefine1 = nPacketDefine1;
	m_nPacketDefine2 = PACKET2_MIN;//用户手动设置，默认为MIN
	m_nPacketSize = nPacketSize;
	m_pNetObject = NULL;
}

void IPackHead::SetNetObject(void *pNetObject)
{
	m_pNetObject = pNetObject;
}
void* IPackHead::GetNetObject()  const
{
	return m_pNetObject;
}
void IPackHead::SetPacketDefine2(PACKET_DEFINE2 nPacketDefine2)
{
	m_nPacketDefine2 = nPacketDefine2;
}
void IPackHead::SetPacketSize(int nPacketSize)
{
	m_nPacketSize = nPacketSize;
}
PACKET_DEFINE1 IPackHead::GetPacketDefine1()  const
{
	return m_nPacketDefine1;
}
PACKET_DEFINE2 IPackHead::GetPacketDefine2()  const
{
	return m_nPacketDefine2;
}


//包大小，head+content
int IPackHead::GetPacketSize()  const
{
	return m_nPacketSize;
}

int IPackHead::GetPacketContentSize()  const
{
	return m_nPacketSize-sizeof(IPackHead);
}



IPacketObject::IPacketObject(PACKET_DEFINE1 nPacketDefine)
{
	m_nPacketDefine = nPacketDefine;
}
PACKET_DEFINE1 IPacketObject::GetPacketID()
{
	return m_nPacketDefine;
}