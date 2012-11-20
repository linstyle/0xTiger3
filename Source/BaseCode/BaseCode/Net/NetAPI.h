/*
	@lindp lin_style@foxmail.com   
	@2012/2/17
	@�����װ����CNetDriver2.h��public�Ĳ���,�����������뿴CNetDriver2.hͷ�ļ�
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