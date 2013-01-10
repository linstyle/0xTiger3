/*  作者：		林东平                                                              
	创建时间：	2010/07/14   5                                                     
    功能说明：  网络包工厂管理，主要是为了屏蔽繁杂的序列号，返回对应的协议处理对象
*/

#pragma once

#include "PPackets.h"


class CPacketFactory
{
public:
	CPacketFactory();
	~CPacketFactory();

	void AddPacketObject(IPacketObject* pPacketObject);
	/*
		ProcessMsg这里返回值不设置为bool.
		因为内部的逻辑错误表示值有很多，比如包不符合，一些其他错误等。但是并不表示
		就要返回false去断开这个连接，其中再做区分太细致。因此是否断开，由内部逻辑
		自己控制
	*/
	void ProcessMsg(IPacketHead *pPacketHead);

	IPacketObject* GetPacketObject( int e );
	

private:
	IPacketObject* GetPacketObject( IPacketHead *pPacketHead );

public:

private:
	bool m_bFactory[PACKET1_MAX];
	IPacketObject* m_Factory[PACKET1_MAX];
	int m_nFactorySize;

};

extern CPacketFactory* g_LoigcPacketFactory;
extern CPacketFactory* g_NetPacketFactory;