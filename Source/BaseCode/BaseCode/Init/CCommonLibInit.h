/*  ���ߣ�		�ֶ�ƽ                                                              
	����ʱ�䣺	2011/1/24                                                      
    ����˵����  CommonLIB��ʼ������
*/

#pragma once

#include "Singleton.h"
#include "CNetDriver2.h"
#include "Clog2.0.h"
#include "CCmInitPackets.h"
#include "CPacketFactory.h"


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

public:

private:


};

#define g_SingletonManager CCmInitSingleton::getSingleton()