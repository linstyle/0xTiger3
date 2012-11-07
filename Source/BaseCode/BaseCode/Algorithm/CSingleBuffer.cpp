
#include "CSingleBuffer.h"

CSingleBuffer::CSingleBuffer()
{
	m_pBuffer = NULL;
	m_nTail = m_nHead = 0;
}

CSingleBuffer::~CSingleBuffer()
{
	delete []m_pBuffer;
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
/*

*/
void CSingleBuffer::ReSet()
{
	m_nTail = m_nHead = 0;
}

char* CSingleBuffer::GetUseBuffer()
{
	return m_pBuffer+m_nTail;
}

char* CSingleBuffer::GetFreeBuffer()
{
	return m_pBuffer+m_nHead;
}

void CSingleBuffer::WriteBuffer(const char* Buffer, int nLen)
{
	char *p = GetFreeBuffer();
	memcpy(p, Buffer, nLen);
}

void CSingleBuffer::WriteFlush(int nLen)
{
	assert( (m_nHead+nLen)<=m_nBufferLen);
	m_nHead += nLen;
}

void CSingleBuffer::ReadBuffer(char *Buffer, int nLen)
{
	const char *p = GetUseBuffer();	

	memcpy(Buffer, p, nLen);
}
	
void CSingleBuffer::ReadFlush(int nLen)
{
	assert( (m_nTail + nLen)<=m_nHead );

	m_nTail += nLen;

	if( true==IsEmpty() )
	{
		ReSet();
	}

}

bool CSingleBuffer::IsEmpty()
{
	return 0==GetUseLength();
}

bool CSingleBuffer::IsFull()
{
	return 0==GetFreeLengthWrite();
}