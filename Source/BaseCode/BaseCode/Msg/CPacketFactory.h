/*  作者：		林东平                                                              
	创建时间：	2010/07/14   5                                                     
    功能说明：  网络包工厂管理，主要是为了屏蔽繁杂的序列号，返回对应的协议处理对象
*/

#pragma once


#include "Singleton.h"
#include "IPackets.h"


class CPacketFactory:public Singleton<CPacketFactory>
{
public:
	CPacketFactory();
	~CPacketFactory();

	void AddPacketObject(IPacketObject* pPacketObject);
	/*
		ProcessMsg
		返回值
		false:表示出了很严重的问题，pPackHead所对应的网络套接字需要关闭
	*/
	bool ProcessMsg(IPackHead *pPackHead);

	IPacketObject* GetPacketObject( int e );
	

private:
	IPacketObject* GetPacketObject( IPackHead *pPackHead );

public:

private:
	bool m_bFactory[PACKET1_MAX];
	IPacketObject* m_Factory[PACKET1_MAX];
	int m_nFactorySize;

};

#define g_PacketFactory CPacketFactory::getSingleton()