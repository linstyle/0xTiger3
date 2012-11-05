/*  
	作者：		林东平                                                              
	创建时间：	2010/07/14                                                         
    功能说明：  需要和网络层打交道的套接字，然后转成逻辑套接字
				Physical负责实际上与网络层打交道的动作和API，比如连接，recv,send
				，当套接字已经有数据之后，那么在在应用层上只是对其缓冲区中的数据
				进行存取，然后根据数据的内容进行做操作。发送数据亦是如此。
				因为对外暴露的是Physical的接口，所以Physical冗余了一些管理上的接口。
				当然也可以单独写一个类似管理的东西

*/

/*
			SendFlush / del/add(gc+-)
	PhysicalSocket	---RecvFlush(相当GetIOCP)(gc) del/add(gc+-)
		
	LogicSocket		--RecvFlush(gc) del/add(gc+-)
					--ExceptionFlush
		
*/

#pragma once

#include <vector>

#include "GlobalType.h"
#include "Singleton.h"
#include "CIOCP.h"
#include "ILocalObject.h"
#include "CSocketLogic.h"
#include "CNetTracer.h"

#include "CPacketFactory.h"
#include "CSlab.h"

#include "OSOther.h"


/*
	被激活的终端工作集合
*/

class CSocketPhysical;
struct THREAD_PARAM
{
	CSocketPhysical *pSocketPhysical;
	int i;	
};

namespace name_socket_physical
{
	struct THREAD_PARAM_ACCPET
	{
		bool *pRun;
		CSocketPhysical *pSocketPhysical;
		ILocalObjectAccept *pObjectAccept;
	};

	struct THREAD_PARAM_CONNECT
	{
		bool *pRun;
		CSocketPhysical *pSocketPhysical;
	};
};

/*
	执行顺序：
	AddLocalObjectConnect, AddLocalObjectAccept,本地套接字的添加
	Fire(),IOCP等线程开始工作
	Run(),时间片运行接口可以抛在一个线程中执行，也可以主线程调用
*/
class CSocketPhysical
{
public:
	CSocketPhysical();
	~CSocketPhysical();

	void TracerMe();
	void Run();

	void AddLocalObjectConnect(ILocalObjectConnect* pLocalObject);
	void AddLocalObjectAccept(ILocalObjectAccept* pLocalObject);
	void Fire();	

	/*
		connect,accept线程调用
	*/
	void DelIOCPRecv(CIOCPData* pIOCPKey, bool bException=false);
	
private: 
	/*
		接收、连接的线程
	*/
	static unsigned int WINAPI ProcAccept(void* param);
	static unsigned int WINAPI ProcConnect(void* param);
	std::vector<HANDLE> m_hThreadAccept;
	std::vector<HANDLE> m_hThreadConnect;

	/*
		线程启动和关闭
	*/
	bool StartIOCP();
	bool StartThreadAccept();
	bool StartThreadConnect();
	void EndThreadAccept();
	void EndThreadConnect();

	/*
		对异常套接字的处理
	*/
	void FlushException();
	
	/*
		Recv线程(本身)执行的动作
		FramsStatus暂时以取不到任务队列做为收取的停止
	*/
	void RunStatus();
	bool FramStatus();
	void AcceptConnect(CIOCPData* pIOCPKey);
	
	/*
		这里的AddIOCPRecv指的是让完成端口GetStatus的动作，有Accept,Connect,WSARecv
		其中WSARecv自带有post的动作，不调用该函数
	*/
	int AddIOCPRecv(CIOCPData* pIOCPKey);
	
	/*
		对象缓冲池的一个封装
	*/
	CIOCPData* MallocNetObject();
	void FreeNetObject(CIOCPData* p);

	/*
		Close
	*/
	void CloseSocket(CIOCPData* pIOCPKey);
	int  CloseLocalObject(CIOCPData* pIOCPKey);
	void CloseAllSocket();

public:
	CIOCP m_IOCP;

private:
	CSocketLogic m_SocketLogic;

	/*
		存放本地的连接
	*/
	std::vector<ILocalObjectAccept*> m_LocalObjectAccept;
	std::vector<ILocalObjectConnect*> m_LocalObjectConnect;

	bool m_bThreadRun;
};

