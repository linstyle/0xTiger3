/*
	@lindp lin_style@foxmail.com   
	@2012/2/17
	@纯粹封装了下CNetDriver2.h中public的部分,函数名解释请看CNetDriver2.h头文件
*/
#pragma once
#include "CNetDriver2.h"

namespace net  
{
	bool SetSocketServer(const char* pName, const char* pListenIP, USHORT nListenPort);	
	bool AddConnectSocket(const char* pConnectIP, USHORT nConnectPort);
	void Init();

	int SendPacket(IPacketHead* pPackHead);
	bool CloseNet(unsigned int nNetKey);
};