/*  
	作者：		林东平                                                              
	创建时间：	2010/09/18                                                         
    功能说明：  单开线程+一个双缓冲区
				命名方式: xxx时间(xxx表示用户传入的字符)
*/

#pragma once
#include <windows.h>
#include <process.h>
#include <stdio.h> 
#include <vector>


#include "MTASSERT.h"
#include "CCircleBuffer.h"
#include "CTime.h"

#define __WFILE__2(x) L ## x
#define __WFILE__1(x) __WFILE__2(x)
#define __WFILE__ __WFILE__1(__FILE__)

#define __WFUNCTION__2(x) L ## x
#define __WFUNCTION__1(x) __WFUNCTION__2(x)
#define __WFUNCTION__ __WFUNCTION__1(__FUNCTION__)

#define __WLINE__2(x) L ## x
#define __WLINE__1(x) __WLINE__2(x)
#define __WLINE__ __WLINE__1(__LINE__)

namespace name_log
{
	const int FLUSH_INTERVAL_SECONDS = 1;				//写到保存终端的间隔秒数

	const int CREATE_SAVE_SIZE = 10*1024*1024;			//mb,当文件超过时，重新建立文件
	const int SAVE_PATH_LEN = 128;							   //路径长度，路径+文件名
	const int SAVE_NAME_POSTFIX_LEN = 16;				       //保存的文件名里时间的长度+后缀

	const int LINE_BUFFER_LEN = 32*1024;					   //kb,单次允许写入临时缓冲区的大小
	const int CIRCLE_BUFFER_LEN = 5*1024*1024;			//mb,环形缓冲区大小,写入到终端前的临时存放空间
	/*
		输出终端的控制
		32位的bit,高16位表示输出终端、输出详细等控制，
				  低16位表示消息等级
		判断语法: if( PUT_MASK_XX & PUT_MASK )
				  {
						输出到XX终端
				  }
	*/
	const int PUT_MASK_CONSOLE = 0x10000;		//控制台
	const int PUT_MASK_HD       =     0x100000;		//磁盘
	const int PUT_MASK_DETAIL =   0x1000000;		//是否详细

	const int PUT_MASK = PUT_MASK_HD|PUT_MASK_DETAIL;
};

/*
	输出等级
*/
enum LOG_LEVEL
{
	//确保|后的顺序，这里供CLOGLEVEL做为个数初始化
	LOG_NOTIC = name_log::PUT_MASK|0,			//普通输出
	LOG_WARN =  name_log::PUT_MASK|1,		//警告
	LOG_ERROR = name_log::PUT_MASK|2	,		//出错
	LOG_MAX														//&0xffff0000
};

class CLog;

class CLogThread
{
public:
	CLogThread();
	~CLogThread();

	void Init(CLog *pLog, int nLogCount);
	//void Reg(CLog *pLog);
	void Flush(CLog *pLog);
	void UpdateFlushTime();

private:
	static unsigned int WINAPI ProcWrite(void* param);

	bool IsThreadRun();
	void InitThread();
	void EndThread();

	bool IsCanFlush();
	
	__time64_t GetFlushTime();
	
	int GetLogCount()
	{
		return m_LogVt.size();
	}

	CLog* GetLogAt(int i)
	{
		return m_LogVt[i];
	}

public:

private:
	bool m_bThreadRun;
	HANDLE m_hThread;
	unsigned int m_idThread;

	std::vector<CLog*> m_LogVt;
	__time64_t m_TimeFlushInterval;
};

/*
	控制日志文件名的变化
*/
class CLogPath
{
public:
	void Init(wchar_t *pSaveName, wchar_t *pSavePath);

	bool IsReCreate();
	//刷新下路径名
	void UpdatePath();
	wchar_t * GetLogPath();
private:
	void InitPath(wchar_t *pSaveName, wchar_t *pSavePath);
	void InitTime();

public:

private:
	wchar_t *m_pSavaNamePostFix;										//后缀指针，包括时间+后缀
	wchar_t   m_PostFix[8];													//后缀名
	wchar_t   m_SavePath[name_log::SAVE_PATH_LEN];		//完整路径

	int m_nHour;		//上回的时间,小时
};

class CLog
{
public:
	~CLog();
	void Init(wchar_t *pSaveName, wchar_t *pSavePath=L"");
	void TracerMe();
	
	/*
		如果对变长参数进行多次封装，那么参数应该写成va_list类型进行传递
		同时，谁解析的变长参数由谁来释放
	*/
	int Write(LOG_LEVEL Level, wchar_t* format, va_list VaList);
	int Flush();
	
	void ReCreateSaveTerminal();
	unsigned long SizeSaveTerminal();

	wchar_t* GetSavePath();

private:
	void CreateSaveTerminal();
	void CloseSaveTerminal();
	void InitSaveBuffer();

public:

private:
	//路径的管理
	CLogPath m_LogPath;

	//缓冲区
	CCircleBuffer m_CircleBuffer;
	//当flush时，从m_CircleBuffer中拷贝出来的临时区
	char *m_pFlushTempBuffer;	

	//输出终端句柄
	FILE* m_fPutDevice;

	//保存路径
	wchar_t* m_pSavaPath;

	//tracer
	__int64  m_nSumWriteByte;
}; 

