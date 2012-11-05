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
		m_SingleBuffer[i].InitBuffer(nBufferLenBytes);
	}

	m_nWriteIndex = m_nReadIndex = 0;
}

int CCircleBuffer::WriteBuffer(const char*Buffer, int nLen)
{
	m_LockWrite.Lock();

	int nWrite1 = m_nWriteIndex;
	int nWrite2 = (m_nWriteIndex+1) & 0x1;
	const char *pDst = Buffer;

	int nLen1 = m_SingleBuffer[nWrite1].GetFreeLengthWrite();
	int nLen2 = m_SingleBuffer[nWrite2].GetFreeLengthWrite();
	if( (nLen1+nLen2)<nLen )
	{
		m_LockWrite.UnLock();
		return -1;
	}

	nLen2 = nLen-nLen1;
	if( nLen2<=0 )
	{
		/*
			说明nLen1刚好可以容纳下nLen		
		*/
		m_SingleBuffer[nWrite1].WriteBuffer(Buffer, nLen);
		goto goto_write_end;
	}

	m_SingleBuffer[nWrite1].WriteBuffer(Buffer, nLen1);
	m_SingleBuffer[nWrite2].WriteBuffer(Buffer, nLen2);

goto_write_end:
	WriteBufferFlush(nLen);
	m_LockWrite.UnLock();

	return 0;
}


int CCircleBuffer::ReadBuffer(char *Buffer, int nLen)
{
	m_LockRead.Lock();

	int nRead1 = m_nReadIndex;
	int nRead2 = (m_nReadIndex+1) & 0x1;
	char *pDst = Buffer;

	int nLen1 = m_SingleBuffer[nRead1].GetUseLength();
	int nLen2 = m_SingleBuffer[nRead2].GetUseLength();
	if( (nLen1+nLen2)<nLen )
	{
		m_LockRead.UnLock();
		return -1;
	}

	nLen2 = nLen-nLen1;
	if( nLen2<=0 )
	{
		/*
			说明nLen1刚好可以容纳下nLen		
		*/
		m_SingleBuffer[nRead1].ReadBuffer(Buffer, nLen);
		goto goto_read_end;
	}

	m_SingleBuffer[nRead1].ReadBuffer(Buffer, nLen1);
	m_SingleBuffer[nRead2].ReadBuffer(Buffer, nLen2);

goto_read_end:
	ReadBufferFlush(nLen);
	m_LockRead.UnLock();

	return 0;
}

void CCircleBuffer::WriteBufferFlush(int nLen)
{
	int nWrite1 = m_nWriteIndex;
	int nWrite2 = (m_nWriteIndex+1) & 0x1;

	int nLen1 = m_SingleBuffer[nWrite1].GetFreeLengthWrite();
	int nLen2 = m_SingleBuffer[nWrite2].GetFreeLengthWrite();

	nLen2 = nLen-nLen1;
	if( nLen2<=0 )
	{
		/*
			说明nLen1刚好可以容纳下nLen		
		*/
		m_SingleBuffer[nWrite1].WriteFlush(nLen);
		return;
	}

	m_SingleBuffer[nWrite1].WriteFlush(nLen1);
	m_SingleBuffer[nWrite2].WriteFlush(nLen2);
	/*
		这里索引改变
	*/
	m_nWriteIndex = nWrite2;
}

void CCircleBuffer::ReadBufferFlush(int nLen)
{
	int nRead1 = m_nReadIndex;
	int nRead2 = (m_nReadIndex+1) & 0x1;

	int nLen1 = m_SingleBuffer[nRead1].GetUseLength();
	int nLen2 = m_SingleBuffer[nRead2].GetUseLength();

	nLen2 = nLen-nLen1;
	if( nLen2<=0 )
	{
		/*
			说明nLen1刚好可以容纳下nLen		
		*/
		m_SingleBuffer[nRead1].ReadFlush(nLen);
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