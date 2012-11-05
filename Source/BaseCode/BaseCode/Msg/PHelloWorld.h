/*  作者：		林东平                                                              
	创建时间：	2012/3/12                                                        
    功能说明：  对登陆的对象，都会发送一个hello world
					   Object实例需要在CCmInitPackets.h中初始化全局
*/

#pragma once


#include "Singleton.h"
#include "IPackets.h"
#include "CPacketFactory.h"

//这里需要修改
#define _PacketID1 PACKET1_HELLO


class PHelloWorld:public IPackHead
{
public:
	char m_Buffer[512];
	PHelloWorld():IPackHead(_PacketID1, sizeof(this))
	{
		STATIC_ASSERT( sizeof(PHelloWorld)<name_msg_packet::SOCKET_BUFF_SIZE );		
		memset(m_Buffer, 0, sizeof(m_Buffer));
	}
};


class PHelloWorldObject:public Singleton<PHelloWorldObject>, public IPacketObject
{ 
public:
	PHelloWorldObject():IPacketObject(_PacketID1)
	{
		g_PacketFactory.AddPacketObject(this);
	}

	virtual int	Execute(IPackHead* pPackHead) ;
};