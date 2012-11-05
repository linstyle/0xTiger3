#include "StdAfx.h"
#include "CSocketSendBuffer.h"


CSocketSendBuffer::CSocketSendBuffer(int nBufferLen):CCircleBuffer(nBufferLen)
{

}

const char* CSocketSendBuffer::GetReadBuffer(int *pWishLen)
{
	//printf("CSocketSendBuffer::ReadBuffer:%d \n", m_nReadIndex);
	*pWishLen = m_SingleBuffer[m_nReadIndex].GetUseLength();
	const char *pBuffer = m_SingleBuffer[m_nReadIndex].GetUseBuffer();
	
	return pBuffer;
}

void CSocketSendBuffer::ReadBufferFlush(int nLen)
{
	CCircleBuffer::ReadBufferFlush(nLen);
}


void CSocketSendBuffer::TracerMe()
{
//	printf("Notice, Buffer1 tail:%d, head:%d\n", m_SingleBuffer[0].m_nTail, m_SingleBuffer[0].m_nHead);
//	printf("Notice, Buffer0 tail:%d, head:%d\n", m_SingleBuffer[1].m_nTail, m_SingleBuffer[1].m_nHead);
}