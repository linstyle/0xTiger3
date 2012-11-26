/*  作者：		林东平                                                              
	创建时间：	2011/1/24                                                      
    功能说明：  CommonLIB初始化管理
*/

#pragma once


#include "Singleton.h"
#include "CNetDriver2.h"
#include "Clog2.0.h"
#include "CPacketFactory.h"
#include "CCmInitPackets.h"
#include "CLogicDriver.h"
//#include "CThreadPool.h"
//#include "CThreadQuit.h"
//#include "CNetTransToLogic.h"
//#include "CNetDriver.h"

//#include "CWindowsSlabManager.h"


//#include "CNetTaskQueue.h"

//#include "CObjectMapManager.h"
//#include "CLogicDriver.h"


class CCommonLibInit:public Singleton<CCommonLibInit>
{
public:
	CCommonLibInit();
	~CCommonLibInit();

private:
	//一些独立库的初始化放这
	void InitSocketLib();	
	void ReleaseSocketLib();


public:

private:


};

#define g_SingletonManager CCommonLibInit::getSingleton()
