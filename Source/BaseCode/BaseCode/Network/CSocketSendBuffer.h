/*  
	作者：		林东平                                                              
	创建时间：	2010/08/09                                                         
    功能说明：  两个单反构成的类似环形缓冲区
*/
#pragma once

#include "GlobalType.h"
#include "CCircleBuffer.h"


namespace name_socket_send_buffer
{
	const int DEFAULT_BUFFER_LEN = 1024*32; //kb
};

class CSocketSendBuffer:public CCircleBuffer
{
public:
	CSocketSendBuffer(int nBufferLen = name_socket_send_buffer::DEFAULT_BUFFER_LEN);

	void TracerMe();
	/*
		写入，可直接调用CCircleBuffer的WriteBuffer接口
	*/
	/*
		根据套接字需求，做一些特殊的封装
		获取发送缓冲的指针和长度，直接发送。
	*/
	const char* GetReadBuffer(int *pWishLen);
	void ReadBufferFlush(int nLen);

	

private:


public:

private:
};
