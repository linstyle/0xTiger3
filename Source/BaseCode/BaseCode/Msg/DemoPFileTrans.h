/*  作者：		林东平                                                              
	创建时间：	2010/08/02                                                        
    功能说明：  文件传输的一个例子
*/
#pragma once

#include "IPackets.h"
#include "CPacketFactory.h"

//这里需要修改
#define _PacketID1 PACKET1_FILE_TRANS

//这个类是数据结构
class PFileTrans:public IPackHead
{
public:	
	char m_Buffer[1024];
	unsigned int m_BufferLen;
	bool m_bEnd;
	
	PFileTrans():IPackHead(_PacketID1, sizeof(this))
	{
		memset(m_Buffer, 0, 1024);
		m_BufferLen = 0;
		m_bEnd = false;
		
		//主要做静态的检查
		STATIC_ASSERT( sizeof(this)<name_msg_packet::SOCKET_BUFF_SIZE );		
	}
};

//这个类实现Execute即可
class PFileTransObject:public Singleton<PFileTransObject>, public IPacketObject
{
public:
	PFileTransObject():IPacketObject(_PacketID1)
	{
		g_PacketFactory.AddPacketObject(this);
	}

	
	virtual int	Execute(IPackHead* pPackHead, void* pRole) ;
};