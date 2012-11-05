/*  
	作者：		林东平                                                              
	创建时间：	2010/08/13  深夜                                                         
    功能说明：  构成环形缓冲区的单反结构
*/
#pragma once
#include <memory.h>

namespace name_socket_single_buffer
{
	const int DEFAULT_BUFFER_LEN = 1024*32; //kb
};

/*
	单缓冲体
*/
class CSingleBuffer
{
public:
	CSingleBuffer();
	~CSingleBuffer();
	void InitBuffer(int nBytes);

	bool WriteBuffer(const char* Buffer, int nLen);
	void WriteFlush(int nLen);
	/*
		ReadBuffer,ReadFlush,算是一个真正的读取动作
	*/
	const char* ReadBuffer();
	bool ReadFlush(int nLen);
	
	/*
		长度分两种，一种是可写入(FreeLengthWrite)，一种是全部的空闲长度(GetFreeLength)				
	*/
	int GetUseLength();
	int GetFreeLength();
	int GetFreeLengthWrite();
	char* GetFreeBuffer();
	const char* GetUseBuffer();

private:
	void ReSet();
	bool IsCanWrite(int nLen);
	
public:
	/*
		指向环形头尾的计数索引
		m_nTail...........m_nHead
	*/
	int m_nTail;
	int m_nHead;	

private:
	char *m_pBuffer;

	int m_nBufferLen;
};