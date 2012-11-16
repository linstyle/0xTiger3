#include "PPackets.h"
#include "MTASSERT.h"
#include "mystdio.h"


IPackHead::IPackHead()
{
	MEMSET(this, 0, sizeof(IPackHead));
}

//bool IPackHead::InitByCreate(const char* pDesBuffer, unsigned short nBufferSize)
//{
//	IFn(!pDesBuffer)
//	{
//		return false;
//	}	
//
//	if( nBufferSize>NET_PACKET_BUFF_SIZE )
//	{
//		return false;
//	}
//
//	memcpy(m_Buffer, pDesBuffer, nBufferSize);
//
//	return true;
//}

char* IPackHead::GetPacketBuffer()
{
	return m_Buffer;
}

unsigned short IPackHead::GetPacketSize()
{
	return m_PacketHead.m_nPacketSize;
}

unsigned short IPackHead::GetPacketDefine1()
{
	return m_PacketHead.m_nPacketDefine1;
}

unsigned short IPackHead::GetPacketDefine2()
{
	return m_PacketHead.m_nPacketDefine2;
}

/********************************
		IPacketObject
*********************************/
//IPacketObject::IPacketObject(PACKET_DEFINE1 nPacketDefine)
//{
//	m_nPacketDefine = nPacketDefine;
//}
//PACKET_DEFINE1 IPacketObject::GetPacketID()
//{
//	return m_nPacketDefine;
//}