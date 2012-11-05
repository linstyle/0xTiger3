/*  
	作者：		林东平                                                              
	创建时间：	2010/08/19  夜                                                         
    功能说明：  环形缓冲区。这里的双缓不适合套接字结构
*/
#pragma once
#include "CLock.h"
#include "CSingleBuffer.h"

class CCircleBuffer
{
public:
	CCircleBuffer(int nBufferLenBytes);
	CCircleBuffer();
	~CCircleBuffer();

	void Init(int nBufferLenBytes); //KB
	/*
		WriteBuffer
		返回值为-1或者0， 不设置为写入的实际长度。这可以由缓冲区长度控制。
		给出足够的长度，如果还写不满，直接丢弃，不做更复杂的处理
	*/
	int WriteBuffer(const char*Buffer, int nLen);

	/*
		ReadBuffer
		返回值为真正读取的真正长度,该长度小于等于参数nLen
	*/
	int ReadBuffer(char *Buffer, int nLen);

	/*
		返回使用长度
	*/
	int GetUseLength();

private:	
	void WriteBufferFlush(int nLen);
	void ReadBufferFlush(int nLen);

public:

private:
	int m_nWriteIndex;
	int m_nReadIndex;

	CSingleBuffer m_SingleBuffer[2];
	/*
		环形缓冲允许一个线程读，一个线程写，并行，不需要加锁
		如果一个读或者写的动作被多个线程执行，需要加锁
	*/
	CFastLock m_LockWrite;
	CFastLock m_LockRead;


};
