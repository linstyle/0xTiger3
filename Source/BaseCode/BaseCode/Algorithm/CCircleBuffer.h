/*  
	作者：		林东平                                                              
	创建时间：	2010/08/19  夜                                                         
    功能说明：  环形缓冲区。支持多线程的读和写。在读、写各一个线程时并不加锁。
	            多线程下还有很大优化空间，可以并行的写入，暂不实现

	@更新 2012/2/12
	api归类。使用中发现，对需要的api暴露需要两类，
	一类是将内部指针暴露出，减少拷贝:(不支持多线程)
	    write,比如WSARecv可以直接挂钩指针空间.最后调用flush接口
		read,比如写入日志和send接口，只关心取出多少和指针两个参数

	二类是屏蔽内部实现，传入空间和长度等参数，进行拷贝:(支持多线程)
	    普通的写入
		读出又有点不一样，只是一个"偷窥"的动作

	后者涉及到锁的操作，所以两者实现上的代码分开（虽然冗余）
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
		一类API名字，需要配合flush使用.但是因为其指针的暴露需求，不支持多线程
	*/
	char *GetWriteBuffer1(int *pLenBytes);
	char *GetWriteBuffer2(int *pLenBytes);
	void WriteBufferFlush(int nWriteLenBytes);

	char* GetReadBuffer(int *pWishLenBytes);
	int TryReadBuffer(char *pBuffer, int nReadLenBytes);
	void ReadBufferFlush(int nReadLenBytes);

	/*
		二类API名字，加以atom区分，表示是个原子操作：支持多线程
	*/
	/*
		WriteBuffer
		返回值为-1或者0， 要么写成功，要么失败

		ReadBufferAtom
		返回值为-1或者0， 要么读成功，要么失败
	*/
	int WriteBufferAtom(const char* pBuffer, int nWriteLenBytes);
	int ReadBufferAtom(char* pBuffer, int nReadLenBytes);


	int GetUseLength();
private:
	


protected:

public:
	int m_nWriteIndex;
	int m_nReadIndex;

	CSingleBuffer m_SingleBuffer[2];
	/*
		锁芯
	*/
	CFastLock m_LockWrite;
	CFastLock m_LockRead;
};
