/*
   功能描述：定时器 
   文件说明：单线程下使用
			 定时单位	 ：毫秒
			 最长预定时间：136年后
			 使用例子，见该页末尾
   作    者：林东平
   创建时间：2010-09-27
   修    改：
*/

#pragma once

#include "LinuxList.h"
#include <stdio.h>
#include <windows.h>

namespace name_timer
{
	const int VEC_SIZE = 4;
	const int VECS_SIZE = 5;

	const int TVN_BITS = 6;
	const int TVR_BITS = 8;
	const int TVN_SIZE = (1 << TVN_BITS); 
	const int TVR_SIZE = (1 << TVR_BITS);
	const int TVN_MASK = (TVN_SIZE - 1);
	const int TVR_MASK = (TVR_SIZE - 1);
}

/*
	时间值的类型未定，暂时用如下
*/
typedef unsigned long timer_t;

/*
	给用户的节点参数,ListNext不需要关心
*/
struct TIMER_ARG
{ 	
	TIMER_ARG()
	{
		INIT_LIST_HEAD(&ListNext);
	}

	/*
		IntervalTime:绝对时间值，毫秒单位
		CallBackArg: 回调函数的参数
		CallBackFn:  回调函数
	*/
	timer_t IntervalTime;
	unsigned long CallBackArg;
	void (*CallBackFn)(TIMER_ARG* pTimerArg);

	struct list_head ListNext;
}; 

/*
	Vec中，nIndex表示Vec的下标

*/
struct TIMER_VEC 
{ 
	int nIndex;
	struct list_head Vec[name_timer::TVN_SIZE]; 

	TIMER_VEC()
	{
		nIndex = 0;
		for(int i=0; i<name_timer::TVN_SIZE; ++i)
		{
			INIT_LIST_HEAD(Vec+i);
		}	
	}
}; 

struct TIMER_VEC_ROOT 
{
	int nIndex; 
	struct list_head Vec[name_timer::TVR_SIZE];

	TIMER_VEC_ROOT()
	{
		nIndex = 0;
		for(int i=0; i<name_timer::TVR_SIZE; ++i)
		{
			INIT_LIST_HEAD(Vec+i);
		}	
	}
}; 

class CTimer
{
public:
	CTimer();

	void RunTimerList();
	bool AddTimer(TIMER_ARG *pTimer);
	bool DelTimer(TIMER_ARG *pTimer);

	/*
		输出内部信息供调试
	*/
	void TracerMe();

private:
	void Init();
	void InitVecs();
	void InitTime();

	void __AddTimer(TIMER_ARG *pTimer);
	void __DelTimer(TIMER_ARG *pTimer);
	void __RunTimer();
	void UpdateTv();
	int Cascade(TIMER_VEC *pTimerVec, int nIndex);

	void RunCallBackList(list_head *pHead);

	/*
		对前，后两个时间的操作
	*/
public:
	timer_t GetEndTime()
	{
		return m_nNowTime;
	}
	timer_t GetBeginTime()
	{
		return m_nLastTime;
	}
	void UpdateEndTime()
	{	
		m_nNowTime = GetTickCount();
	}
	void UpdateBeginTime(timer_t t)
	{
		m_nLastTime = t;
	}
	/*
		时间差值判断
		a是否在时刻b之后
	*/
	bool TimeAfter(unsigned long a, unsigned long b)
	{
		return ((long)a-(long)b)>0;
	}
	bool TimeBefore(unsigned long a, unsigned long b)
	{
		return TimeAfter(b, a);
	}
	bool TimeAfterEq(unsigned long a, unsigned long b)
	{
		return ((long)a-(long)b)>=0;
	}
	bool TimeBeforeEq(unsigned long a, unsigned long b)
	{
		return TimeAfterEq(b, a);
	}

	/*
		取得下一个等级的vec偏移值
	*/
	int CascadeIndex(int i)
	{
		return ( GetBeginTime() >> (name_timer::TVR_BITS + i*name_timer::TVN_BITS) ) & name_timer::TVN_MASK;
	}

public:

private:
	/*
		毫秒计时
	*/
	timer_t m_nNowTime;
	timer_t m_nLastTime;

	/*
		时间向量
	*/
	TIMER_VEC_ROOT m_TvRoot;
	TIMER_VEC m_Tv[name_timer::VEC_SIZE];

	TIMER_VEC *m_Tvecs[name_timer::VECS_SIZE];

	/*
		debug,计数器
	*/
	int m_nCount[name_timer::VECS_SIZE][name_timer::TVR_SIZE];
};

/*
	使用DEMO
void TimerFun(TIMER_ARG* pTimerArg)
{
	static int i=0;
	int sub = GetTickCount()-pTimerArg->IntervalTime;
	printf("CallBack Sub:%d, Count:%d\n", sub, i++);
}

int _tmain(int argc, _TCHAR* argv[])
{

	//Windows Sleep(),最小精度在20-25毫秒

	int i;
	const int len = 2000;
	CTimer Timer;
	TIMER_ARG arg[len];

	srand( (unsigned)time( NULL ) );

	for(i=0; i<len; ++i)
	{
		arg[i].CallBackFn = TimerFun;
		arg[i].IntervalTime = 500*i + GetTickCount() + rand()%200;
		arg[i].CallBackArg = arg[i].IntervalTime;
		Timer.AddTimer(&arg[i]);	
	}

	while(1)
	{
		Timer.RunTimerList();
		Sleep(20);
	}
}
*/