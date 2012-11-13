/*  作者：		林东平                                                              
	创建时间：	2010/1/19                                                        
    功能说明：  网络层将数据放入任务队列
				Object实例需要在CCmInitPackets.h中初始化全局
    更新
	@2012/2/17
	该类为逻辑层和网络层之间的内部通讯协议
*/

#pragma once

#include "Singleton.h"
#include "PPackets.h"
#include "CPacketFactory.h"

//这里需要修改
#define _PacketID1 PACKET1_INNER_NET_LOGIC


class PNLInnerNotic:public IPackHead
{
public:
	SOCKET m_nSocket;
	USHORT m_nPort;
	ULONG  m_nIP;
	//void*      m_pNetObject;  //移动公共头了
	bool m_bAutoConnect;

	ULONG m_ErrParam;	
	PNLInnerNotic():IPackHead(_PacketID1, sizeof(this))
	{
		m_nSocket = 0;
		m_nPort = 0;
		m_nIP = m_ErrParam = 0;
	//	m_pNetObject = NULL;
		m_bAutoConnect = true;

		STATIC_ASSERT( sizeof(PNLInnerNotic)<name_msg_packet::SOCKET_BUFF_SIZE );		
	}
};


class PNLInnerNoticObject:public Singleton<PNLInnerNoticObject>, public IPacketObject
{ 
public:
	PNLInnerNoticObject():IPacketObject(_PacketID1)
	{
		g_PacketFactory.AddPacketObject(this);
	}

	virtual int	Execute(IPackHead* pPackHead) ;
};
