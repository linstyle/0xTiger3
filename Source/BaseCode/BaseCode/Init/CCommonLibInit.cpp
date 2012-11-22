#include "CCommonLibInit.h"

initialiseSingleton(CCommonLibInit);

//CNetTaskQueue *g_pNetTaskQueue;//网络任务队列

CCommonLibInit::CCommonLibInit()
{

//	g_pNetTaskQueue = new CNetTaskQueue; //网络任务队列
	
	//new CWindowsSlabManager;          //内存池
	//printf("Notic, CCommonLibInit::CCommonLibInit():CWindowsSlabManager() OK\n");

	InitSocketLib();

	new CLogManager2;                       //日志
	printf("Notic, CCommonLibInit::CCommonLibInit():New CLogManager2 OK\n");

	new CNetDriver2;						//网络驱动层
	printf("Notic, CCommonLibInit::CCommonLibInit():New CNetDriver2 OK\n");

	new CPacketFactory;						//包工厂
	printf("Notic, CCommonLibInit::CCommonLibInit():New CPacketFactory OK\n");

	new CCmInitPackets;						//公共协议包
	printf("Notic, CCommonLibInit::CCommonLibInit():New CCmInitPackets OK\n");

	//new CObjectMapManager;              //网络对象管理
	//printf("Notic, CCommonLibInit::CCommonLibInit():New CNetObjectManager OK\n");

	//new CThreadPool;						//线程池	
	//printf("Notic, CCommonLibInit::CCommonLibInit():New CThreadPool OK\n");

	//new CNetTransToLogic;					//网络-逻辑解释层	
	//printf("Notic, CCommonLibInit::CCommonLibInit():New CNetTransToLogic OK\n");

	//new CNetDriver;							//网络-驱动层
	//printf("Notic, CCommonLibInit::CCommonLibInit():New CNetDriver OK\n");

	//new CLogicDriver;						//逻辑驱动层
	//printf("Notic, CCommonLibInit::CCommonLibInit():New CLogicDriver OK\n");

	//new CThreadQuit;						//退出线程
	//printf("Notic, CCommonLibInit::CCommonLibInit():New CThreadQuit OK\n");
}

CCommonLibInit::~CCommonLibInit()
{
	delete CCmInitPackets::getSingletonPtr();  //公共协议包
	printf("Notic, CCommonLibInit::~CCommonLibInit():Delete CCmInitPackets OK\n");

	delete g_PacketFactory.getSingletonPtr();  //包工厂
	printf("Notic, CCommonLibInit::~CCommonLibInit():Delete CPacketFactory OK\n");

	delete g_NetDriver2.getSingletonPtr();      //网络驱动层
	printf("Notic, CCommonLibInit::~CCommonLibInit():Delete CNetDriver2 OK\n");

	Sleep(2000);
	delete CLogManager2::getSingletonPtr();    //日志
	printf("Notic, CCommonLibInit::~CCommonLibInit():Delete CLogManager2 OK\n");

	//delete g_ThreadQuit.getSingletonPtr();		//退出线程
	//printf("Notic, CCommonLibInit::~CCommonLibInit():Delete CThreadQuit OK\n");

	//delete g_LogicDriver.getSingletonPtr();		//逻辑驱动层
	//printf("Notic, CCommonLibInit::CCommonLibInit():Delete CLogicDriver OK\n");

	//delete g_NetDriver.getSingletonPtr();   //网络-驱动层
	//printf("Notic, CCommonLibInit::~CCommonLibInit():Delete CNetDriver OK\n");

	//delete g_NetTransToLogic.getSingletonPtr();  //网络-逻辑解释层	
	//printf("Notic, CCommonLibInit::~CCommonLibInit():Delete CNetTransToLogic OK\n");


	//delete g_ThreadPool.getSingletonPtr();
	//printf("Notic, CCommonLibInit::~CCommonLibInit():Delete CThreadPool OK\n");

	//delete g_NetObjectManager.getSingletonPtr();   //网络对象
	//printf("Notic, CCommonLibInit::~CCommonLibInit():Delete CNetObjectManager OK\n");

	//等待日志写入

//	delete g_pNetTaskQueue;	

	//delete CWindowsSlabManager::getSingletonPtr();
	//printf("Notic, CCommonLibInit::~CCommonLibInit():CWindowsSlabManager OK\n");

	ReleaseSocketLib();
}

void CCommonLibInit::InitSocketLib()
{
	WSADATA WsaData;

	INITASSERT( 0!=WSAStartup(MAKEWORD(2,2),&WsaData) );	
}

void CCommonLibInit::ReleaseSocketLib()
{
	WSACleanup();
}