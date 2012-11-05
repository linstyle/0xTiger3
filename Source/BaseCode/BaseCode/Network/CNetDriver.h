/*  
	作者：		林东平                                                              
	创建时间：	2010/07/14                                                         
    功能说明：  对底层网络的一个接口封装
				注意下初始化顺序，先各种Add，然后调用Fire()，整个网络库执行外部接口是Run();
*/

#pragma once

#include <vector>

#include "GlobalType.h"
#include "Singleton.h"
#include "CSocketRun.h"

class CNetDriver: public Singleton<CNetDriver>
{
public:
	void TracerMe();
	void Tracer();
		
	void Init(ILocalObjectConnect* pObjectConnect, int nConnectCount, 
			  ILocalObjectAccept* pObjectAccept, int nAcceptCount);

	void Run();
	int Send(CIOCPData *pIOCPKey, IPackHead *pPackHead);
private:

public:

private:
	CSocketRun m_SocketRun;

};



#define g_NetDriver CNetDriver::getSingleton()