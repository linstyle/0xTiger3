/*  
	作者：		林东平                                                              
	创建时间：	2010/08/13  深夜                                                         
    功能说明：  构成环形缓冲区的单反结构
*/
#pragma once
#include <assert.h>
#include <stdio.h>
#include <memory.h>

/*
	单缓冲体
*/
class CSingleBuffer
{
public:
	CSingleBuffer();
	~CSingleBuffer();
	void InitBuffer(int nBytes);

	/*
		为适应各种不同的异步读取，把一个完整的动作拆成write,flush.只有执行flush后才算一个完整的
		动作。在flush的接口上，把其中设为一个void，提醒其中的原子性。
		所以把该类设计为SingleBuffer,供用户们重新封装成自己想要的功能
	*/
	void WriteBuffer(const char *Buffer, int nLen);
	void WriteFlush(int nLen);
	bool IsFull();

	void ReadBuffer(char *Buffer, int nLen);
	void ReadFlush(int nLen);
	bool IsEmpty();

	/*
		UseLength,当前缓冲区有效数据
		FreeLength,当前缓冲区全部空闲长度
		FreeLengthWrite,当前缓冲区有效空闲长度
		单位:bytes
	*/
	int GetUseLength();
	int GetFreeLengthWrite();

	/*
		有时候需要直接暴露地址空间
	*/
	char* GetFreeBuffer();
	char* GetUseBuffer();

private:
	void ReSet();
	
public:	

private:
	/*
		指向环形头尾的计数索引
		m_nTail...........m_nHead
	*/
	int m_nTail;
	int m_nHead;

	char *m_pBuffer;

	int m_nBufferLen;
};