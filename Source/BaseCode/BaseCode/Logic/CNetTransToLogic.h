/*  作者：		林东平                                                              
	创建时间：	2010/07/20                                                         
    功能说明：  把网络内容引向逻辑.使得网络代码不显得膨胀，在一些独立方面显得容易修改
*/

#pragma once
#include "Singleton.h"
#include "CPacketFactory.h"
#include "ListArrayMap.h"
#include "CLock.h"

class CNetTransToLogic:public Singleton<CNetTransToLogic>
{
public:
	CNetTransToLogic();
	/*
		接收到一个网络连接、出现异常 网络层的处理
	*/
	int AcceptConnect(CIOCPData* pIOCPKey);
	int ExceptionPacket(CIOCPData* pIOCPKey);

	/*
		返回-1表示逻辑层执行出错，网络层需要对应的处理
	*/
	int ExcutePacket(CIOCPData* pIOCPKey);

private:

public:

private:
	CFastLock m_Lock;
	CListArrayMap<CIOCPData*, void*> m_NetLogicMap;

};

#define g_NetTransToLogic CNetTransToLogic::getSingleton()
