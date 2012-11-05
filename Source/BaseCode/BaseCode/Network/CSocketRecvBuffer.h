/*  
	作者：		林东平                                                              
	创建时间：	2010/08/13  深夜                                                         
    功能说明：  两个单反构成的类似环形缓冲区
*/
#pragma once

#include "GlobalType.h"
#include "CCircleBuffer.h"

namespace name_socket_recv_buffer
{
	const int DEFAULT_BUFFER_LEN = 1024*32; //kb
};

class CSocketRecvBuffer:public CCircleBuffer
{
public:
	CSocketRecvBuffer(int nBufferLen = name_socket_recv_buffer::DEFAULT_BUFFER_LEN);

	void TracerMe();

	/*
		一些特殊的需求:
		1.IOCP需要直接对地址进行写入，所以开放GetWriteBuffer1，GetWriteBuffer2接口
		2.在拆包的时候分为包头和包体两次拆分，当前的设计是包头+包体都读取成功了，才
		  视为一个完整的操作。两次拆分的情况下并不知道当前缓冲区的长度是否正确，所以提供
		  一个TryReadBuffer的接口，不做刷新提交的动作。

		  并且这两个需求都不会出现多线程的并发问题		
	*/
	char *GetWriteBuffer1(int *pLen);
	char *GetWriteBuffer2(int *pLen);
	void WriteBufferFlush(int nLen);

	/*
		其实就是个无锁版、不提交刷新的的ReadBuffer
	*/
	int TryReadBuffer(char *Buffer, int nLen);
	void ReadBufferFlush(int nLen);

private:

public:


private:	
};
