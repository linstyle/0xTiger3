#include "PPackets.h"
#include "MTASSERT.h"

IPackHead::IPackHead()
{
	memset(m_Buffer, 0 ,sizeof(m_Buffer));

	m_pBuffer = m_Buffer;

	m_pBuffer = (char*)((int)m_pBuffer + sizeof(m_nPacketSize));
	m_pBuffer = (char*)((int)m_pBuffer + sizeof(m_nPacketDefine1));
	m_pBuffer = (char*)((int)m_pBuffer + sizeof(m_nPacketDefine2));
}

bool IPackHead::SetPacketDefine1(PACKET_DEFINE1 nPacketDefine1)
{
	IFn( nPacketDefine1>PACKET1_MAX )
	{
		return false;
	}

	m_nPacketDefine1 = nPacketDefine1;
	return true;
}

bool IPackHead::SetPacketDefine2(PACKET_DEFINE2 nPacketDefine2)
{
	IFn( nPacketDefine2>PACKET2_MAX )
	{
		return false;
	}

	m_nPacketDefine2 = nPacketDefine2;
	return true;
}

bool IPackHead::AddPacketSize(int nPacketSize)
{
	if ( (m_nPacketSize+nPacketSize) > PACKET_BUFF_SIZE )
	{
		return false;
	}

	m_nPacketSize += nPacketSize;
	return true;
}

unsigned short IPackHead::GetPacketDefine1()
{
	return m_nPacketDefine1;
}
unsigned short IPackHead::GetPacketDefine2()
{
	return m_nPacketDefine2;
}

char* IPackHead::GetPacketBuffer()
{
	return m_pBuffer;
}


//°ü´óÐ¡£¬head+content
unsigned short IPackHead::GetPacketSize()
{
	return m_nPacketSize;
}

const char* IPackHead::K_GetPacketBuffer()
{
	return m_Buffer;
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