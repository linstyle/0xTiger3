
#include "CCircleBuffer.h"


CCircleBuffer::CCircleBuffer(int nBufferLenBytes)
{
	Init(nBufferLenBytes);
}

CCircleBuffer::CCircleBuffer()
{

}

CCircleBuffer::~CCircleBuffer()
{

}

void CCircleBuffer::Init(int nBufferLenBytes)
{
	for(int i=0; i<2; ++i)
	{
		m_SingleBuffer[i].InitBuffer(nBufferLenBytes/2);
	}

	m_nWriteIndex = m_nReadIndex = 0;
}

char* CCircleBuffer::GetWriteBuffer1(int *pLenBytes)
{
	*pLenBytes = m_SingleBuffer[m_nWriteIndex].GetFreeLengthWrite();
	return m_SingleBuffer[m_nWriteIndex].GetFreeBuffer();
}

char* CCircleBuffer::GetWriteBuffer2(int *pLenBytes)
{
	int nWriteIndex = (m_nWriteIndex+1) & 0x1;

	*pLenBytes = m_SingleBuffer[nWriteIndex].GetFreeLengthWrite();
	return m_SingleBuffer[nWriteIndex].GetFreeBuffer();
}

char* CCircleBuffer::GetReadBuffer(int *pWishLenBytes)
{
	*pWishLenBytes = m_SingleBuffer[m_nReadIndex].GetUseLength();
	char *pBuffer = m_SingleBuffer[m_nReadIndex].GetUseBuffer();

	return pBuffer;
}

int CCircleBuffer::TryReadBuffer(char *pBuffer, int nReadLenBytes)
{
	int nRead1 = m_nReadIndex;
	int nRead2 = (m_nReadIndex+1) & 0x1;
	char *pDst = pBuffer;

	int nLen1 = m_SingleBuffer[nRead1].GetUseLength();
	int nLen2 = m_SingleBuffer[nRead2].GetUseLength();
	if( (nLen1+nLen2)<nReadLenBytes )
	{
		return -1;
	}

	nLen2 = nReadLenBytes-nLen1;
	if( nLen2<=0 )
	{
		/*
			说明nLen1刚好可以容纳下nLen		
		*/
		m_SingleBuffer[nRead1].ReadBuffer(pDst, nReadLenBytes);
		goto goto_read_end;
	}

	m_SingleBuffer[nRead1].ReadBuffer(pDst, nLen1);
	pDst += nLen1;
	m_SingleBuffer[nRead2].ReadBuffer(pDst, nLen2);

goto_read_end:

	return 0;	
}

int CCircleBuffer::WriteBufferAtom(const char* pBuffer, int nWriteLenBytes)
{
	m_LockWrite.Lock();

	int nWrite1 = m_nWriteIndex;
	int nWrite2 = (m_nWriteIndex+1) & 0x1;
	const char *pDst = pBuffer;

	int nLen1 = m_SingleBuffer[nWrite1].GetFreeLengthWrite();
	int nLen2 = m_SingleBuffer[nWrite2].GetFreeLengthWrite();
	if( (nLen1+nLen2)<nWriteLenBytes )
	{
		m_LockWrite.UnLock();
		return -1;
	}

	nLen2 = nWriteLenBytes-nLen1;
	if( nLen2<=0 )
	{
		/*
			说明nLen1刚好可以容纳下nLen		
		*/
		m_SingleBuffer[nWrite1].WriteBuffer(pDst, nWriteLenBytes);
		goto goto_write_end;
	}

	m_SingleBuffer[nWrite1].WriteBuffer(pDst, nLen1);
	pDst += nLen1;
	m_SingleBuffer[nWrite2].WriteBuffer(pDst, nLen2);

goto_write_end:
	WriteBufferFlush(nWriteLenBytes);
	m_LockWrite.UnLock();
	
	return 0;
}


int CCircleBuffer::ReadBufferAtom(char *pBuffer, int nReadLenBytes)
{
	m_LockRead.Lock();

	int nRead1 = m_nReadIndex;
	int nRead2 = (m_nReadIndex+1) & 0x1;
	char *pDst = pBuffer;

	int nLen1 = m_SingleBuffer[nRead1].GetUseLength();
	int nLen2 = m_SingleBuffer[nRead2].GetUseLength();
	if( (nLen1+nLen2)<nReadLenBytes )
	{
		m_LockRead.UnLock();
		return -1;
	}

	nLen2 = nReadLenBytes-nLen1;
	if( nLen2<=0 )
	{
		/*
			说明nLen1刚好可以容纳下nLen		
		*/
		m_SingleBuffer[nRead1].ReadBuffer(pDst, nReadLenBytes);
		goto goto_read_end;
	}

	m_SingleBuffer[nRead1].ReadBuffer(pDst, nLen1);
	pDst += nLen1;
	m_SingleBuffer[nRead2].ReadBuffer(pDst, nLen2);

goto_read_end:
	ReadBufferFlush(nReadLenBytes);
	m_LockRead.UnLock();

	return 0;
}

void CCircleBuffer::WriteBufferFlush(int nWriteLenBytes)
{
	int nWrite1 = m_nWriteIndex;
	int nWrite2 = (m_nWriteIndex+1) & 0x1;

	int nLen1 = m_SingleBuffer[nWrite1].GetFreeLengthWrite();
	int nLen2 = m_SingleBuffer[nWrite2].GetFreeLengthWrite();

	nLen2 = nWriteLenBytes-nLen1;
	if( nLen2<=0 )
	{
		/*
			说明nLen1刚好可以容纳下nLen		
		*/
		m_SingleBuffer[nWrite1].WriteFlush(nWriteLenBytes);
		return;
	}

	m_SingleBuffer[nWrite1].WriteFlush(nLen1);
	m_SingleBuffer[nWrite2].WriteFlush(nLen2);
	/*
		这里索引改变
	*/
	m_nWriteIndex = nWrite2;
}

void CCircleBuffer::ReadBufferFlush(int nLenBytes)
{
	int nRead1 = m_nReadIndex;
	int nRead2 = (m_nReadIndex+1) & 0x1;

	int nLen1 = m_SingleBuffer[nRead1].GetUseLength();
	int nLen2 = m_SingleBuffer[nRead2].GetUseLength();

	nLen2 = nLenBytes-nLen1;
	if( nLen2<=0 )
	{
		/*
			说明nLen1刚好可以容纳下nLen		
		*/
		m_SingleBuffer[nRead1].ReadFlush(nLenBytes);
		if( true==m_SingleBuffer[nRead1].IsEmpty() )
		{
			m_nReadIndex = m_nWriteIndex;
		}
		return;
	}

	m_SingleBuffer[nRead1].ReadFlush(nLen1);
	m_SingleBuffer[nRead2].ReadFlush(nLen2);

	/*
		这里索引改变
	*/
	m_nReadIndex = nRead2;
}

int CCircleBuffer::GetUseLength()
{
	int nRead1 = m_nReadIndex;
	int nRead2 = (m_nReadIndex+1) & 0x1;

	return m_SingleBuffer[nRead1].GetUseLength() + m_SingleBuffer[nRead2].GetUseLength();
}

