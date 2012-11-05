#include "StdAfx.h"
#include "CSocketRecvBuffer.h"


CSocketRecvBuffer::CSocketRecvBuffer(int nBufferLen):CCircleBuffer(nBufferLen)
{
		
}

char *CSocketRecvBuffer::GetWriteBuffer1(int *pLen)
{
	*pLen = m_SingleBuffer[m_nWriteIndex].GetFreeLengthWrite();
	return m_SingleBuffer[m_nWriteIndex].GetFreeBuffer();
}

char *CSocketRecvBuffer::GetWriteBuffer2(int *pLen)
{
	int nWriteIndex = (m_nWriteIndex+1) & 0x1;

	*pLen = m_SingleBuffer[nWriteIndex].GetFreeLengthWrite();
	return m_SingleBuffer[nWriteIndex].GetFreeBuffer();
}


void CSocketRecvBuffer::WriteBufferFlush(int nLen)
{
	CCircleBuffer::WriteBufferFlush(nLen);
}

int CSocketRecvBuffer::TryReadBuffer(char *Buffer, int nLen)
{
	int nRead1 = m_nReadIndex;
	int nRead2 = (m_nReadIndex+1) & 0x1;
	char *pDst = Buffer;

	int nLen1 = m_SingleBuffer[nRead1].GetUseLength();
	int nLen2 = m_SingleBuffer[nRead2].GetUseLength();
	if( (nLen1+nLen2)<nLen )
	{
		return -1;
	}

	nLen2 = nLen-nLen1;
	if( nLen2<=0 )
	{
		/*
			说明nLen1刚好可以容纳下nLen		
		*/
		m_SingleBuffer[nRead1].ReadBuffer(pDst, nLen);
		goto goto_read_end;
	}

	m_SingleBuffer[nRead1].ReadBuffer(pDst, nLen1);
	pDst += nLen1;
	m_SingleBuffer[nRead2].ReadBuffer(pDst, nLen2);

goto_read_end:

	return 0;	
}

void CSocketRecvBuffer::ReadBufferFlush(int nLen)
{
	CCircleBuffer::ReadBufferFlush(nLen);
}

void CSocketRecvBuffer::TracerMe()
{
//	printf("Notice, Buffer1 tail:%d, head:%d\n", m_SingleBuffer[0].m_nTail, m_SingleBuffer[0].m_nHead);
//	printf("Notice, Buffer0 tail:%d, head:%d\n", m_SingleBuffer[1].m_nTail, m_SingleBuffer[1].m_nHead);
}

