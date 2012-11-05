/*  作者：		林东平                                                              
	创建时间：	2011/03/08                                                       
    功能说明：  网关登陆
*/

#pragma once
#include "Singleton.h"
#include "IPackets.h"
#include "CPacketFactory.h"

#define _PacketID PACKET_LOGIN_DG

class PLoginDG:public IPackHead
{
public:
	PLoginDG():IPackHead(_PacketID)
	{
		STATIC_ASSERT( sizeof(PLoginDG)<name_msg_packet::SOCKET_BUFF_SIZE );		
	}
};


class PLoginDGObject:public Singleton<PLoginDGObject>, public IPacketObject
{
public:
	PLoginDGObject():IPacketObject(_PacketID, sizeof(PLoginDG))
	{
		g_PacketFactory.AddPacketObject(this);
	}


	virtual int	Execute(IPackHead* pPackHead, void* pRole) ;
};