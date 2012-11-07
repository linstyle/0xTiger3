/*  作者：		林东平                                                              
	创建时间：	2010/06/21                                                      
    功能说明：  该类只是对服务器的各个线程进行一个统一的管理和分配。不做非常复杂的增、删、改等动作，
				        也不做各个任务的线程负载平衡
    注意事项：  继承IThread的类，生存空间需要是new的，并且外部不用删除，托管给ThreadPool
*/

#pragma once

#include <vector>

#include "Singleton.h"
#include "CThread.h"
#include "CTime.h"
#include "MTASSERT.h"

struct THREAD_POOL_OBJECT
{
	THREAD_POOL_OBJECT();

	CThread *pThread;  //用户的this指针
	DWORD m_dwStartTime;
	char m_ThreadName[32];
};

typedef std::vector<THREAD_POOL_OBJECT*> ThreadSet_t;

class CThreadPool:public Singleton<CThreadPool>
{
public:
	~CThreadPool();
	/*
		函数功能:输出线程中的信息
	*/
	void Tracer();


	/*
		函数参数:ExecutionTarget:派生IThread的类， 程序类自动对ExecutionTarget回收
					  ThreadName:允许传递一个名字长度为16的线程字符串
		函数功能:执行一个任务线程
		函数返回: 0 成功
				 -1 失败
	*/
	void ExecuteTask(CThread* ExecutionTarget, const char* ThreadName, int nThreadNumber=1);
private:
	/*
		函数功能:启动一个线程
		函数返回:NULL 失败
				 有值 生成的一个ThreadCtrl对象，并且已经存放到m_ThreadSet中
	*/
	THREAD_POOL_OBJECT* StartThread(CThread* ExecutionTarget, const char* ThreadName);

	/*
		函数功能:退出所有线程
		函数返回: 0 成功
				 -1 失败
	*/
	int Exit();

public:
	

private:
	ThreadSet_t m_ThreadSet;
};


#define g_ThreadPool CThreadPool::getSingleton()
