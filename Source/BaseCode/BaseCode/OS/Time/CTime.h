/*  
	作者：		林东平                                                              
	创建时间：	2010/09/24                                                         
    功能说明：  时间嘛，ftime
	2011/08/13
	修改下接口，用GetXX表示
	YY:年 MM:月 DD:日 HH:小时 MM:分 SS:秒
*/

#pragma once
#include <memory.h>
#include <sys/timeb.h>
#include <time.h>
#include <stdio.h>

struct TIMEW_HHMMSS
{
	TIMEW_HHMMSS()
	{
		memset(m_Buffer, 0, sizeof(m_Buffer));
	}
	wchar_t m_Buffer[16];
};

struct TIME_HHMMSS
{
	TIME_HHMMSS()
	{
		memset(m_Buffer, 0, sizeof(m_Buffer));
	}
	char m_Buffer[16];
};

class CTime
{
public:
	//获得年年月月日日时时分分
	static unsigned long GetYYMMDDHHMM();       
	//月日时
	static unsigned long GetMMDDHH();  
	//时分秒
	static TIMEW_HHMMSS* GetWHHMMSS(TIMEW_HHMMSS *pTimeWHHMMSS);  
	//时分秒
	static TIME_HHMMSS* GetHHMMSS(TIME_HHMMSS *pTimeHHMMSS);  
	//活得当前的小时
	static unsigned long GetHH();
	//获得秒，从1970年开始
	static __time64_t GetSS();		
	//获得毫秒，从1970年开始
	static __time64_t GetMilli();		

private:

public:

private:

};
