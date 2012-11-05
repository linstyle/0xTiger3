#include "CSocketSingleBuffer.h"



CSingleBuffer::CSingleBuffer()
{
	m_nTail = m_nHead = 0;
}

CSingleBuffer::~CSingleBuffer()
{

	delete []m_pBuffer;
}

int CSingleBuffer::GetFreeLength()
{
	return m_nTail+(m_nBufferLen-m_nHead);
}

int CSingleBuffer::GetUseLength()
{
	return m_nHead-m_nTail;
}

int CSingleBuffer::GetFreeLengthWrite()
{
	return m_nBufferLen-m_nHead;
}

void CSingleBuffer::InitBuffer(int nBytes)
{
	m_pBuffer = new char[nBytes];
	m_nBufferLen = nBytes;
	memset(m_pBuffer, 0, nBytes*sizeof(char));

}

void CSingleBuffer::ReSet()
{
	m_nTail = m_nHead = 0;
}

bool CSingleBuffer::IsCanWrite(int nLen)
{
	if( GetFreeLengthWrite()>=nLen )
	{
		return true;
	}

	return false;
}

const char* CSingleBuffer::GetUseBuffer()
{
	return m_pBuffer+m_nTail;
}

char* CSingleBuffer::GetFreeBuffer()
{
	return m_pBuffer+m_nHead;
}

bool CSingleBuffer::WriteBuffer(const char* Buffer, int nLen)
{
	if( false==IsCanWrite(nLen) )
	{		
		return false;
	}

	char *p = GetFreeBuffer();
	memcpy(p, Buffer, nLen);

	
	return true;
}

void CSingleBuffer::WriteFlush(int nLen)
{
	m_nHead += nLen;
}

const char* CSingleBuffer::ReadBuffer()
{
	const char *p = GetUseBuffer();	
	return p;
}
	
bool CSingleBuffer::ReadFlush(int nLen)
{
	m_nTail += nLen;
	if( 0==GetUseLength() )
	{
		ReSet();
		return true;
	}

	return false;
}