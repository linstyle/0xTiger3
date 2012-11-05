/*  作者：		林东平                                                              
	创建时间：	2010/06/21                                                      
    功能说明：  线程的基本结构封装

	@2012/01/19 重构了下代码
	@2012/01/20 感觉线程代码很鸡肋。CommonLib里面几乎都不用到。。除了测试的时候看看线程时间
*/

#pragma once

#include <windows.h>

class CThread
{
public:
	CThread();
	virtual ~CThread();

	/*
		用户实现的接口
	*/
	virtual int Run()=0;	
	virtual int Exit()=0;

	/*
		系统接口
	*/
	DWORD Suspend();
	DWORD Resume();
	DWORD Join();

private:

public:
	HANDLE m_hThread;
	unsigned int   m_uThread;

private:

};