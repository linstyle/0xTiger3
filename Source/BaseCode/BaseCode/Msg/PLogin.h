/*  作者：		林东平                                                              
	创建时间：	2012/3/7                                                        
    功能说明：  GS向LS登陆
					   Object实例需要在CCmInitPackets.h中初始化全局
*/

#pragma once

#include "Singleton.h"
#include "PPackets.h"
#include "CPacketFactory.h"

//这里需要修改
#define _PacketID1 PACKET1_LOGIN_GL


class PLogin:public IPackHead
{
public:

	PLogin():IPackHead(_PacketID1, sizeof(this))
	{

		STATIC_ASSERT( sizeof(PLogin)<name_msg_packet::SOCKET_BUFF_SIZE );		
	}
};


class PLoginGLObject:public Singleton<PLoginGLObject>, public IPacketObject
{ 
public:
	PLoginGLObject():IPacketObject(_PacketID1)
	{
		g_PacketFactory.AddPacketObject(this);
	}

	virtual int	Execute(IPackHead* pPackHead) ;
};
