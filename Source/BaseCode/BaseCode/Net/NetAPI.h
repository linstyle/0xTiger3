/*
	@lindp lin_style@foxmail.com   
	@2012/2/17
	@纯粹封装了下CNetDriver2.h中public的部分,函数名解释请看CNetDriver2.h头文件
*/
#pragma once
#include "CNetDriver2.h"

namespace net  
{
	bool AddSocketServer(const char* pName, const char* pListenIP, USHORT nListenPort);	
	bool AddClientSocket(const char* pConnectIP, USHORT nConnectPort);

	int GetPacketStream(char *pBuffer, int nBufferLen);
	int PutPacketStream(const char *pBuffer, int nBufferLen);

	bool NoticNetErr();
};