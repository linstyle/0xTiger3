/*  
	作    者：林东平                                                              
	创建时间：2012/1/5 
    功能说明：日志，练手中
*/
/*
宏包装


单独线程->环形缓冲->写入
写入数据-环形缓存


文件名->跟当前比对->找到句柄实例，如果没有，新建

句柄实例->写入接口
        ->判断文件重建策略

关闭文件-创建文件
*/
#pragma once
#include <wchar.h>
#include <map>
#include <string>

#include "CTime.h"
#include "CCircleBuffer.h"
#include "Singleton.h"
using namespace std;

class CLogManager2;
/*
	自定义的LOG宏

	默认的有LOGN/W/E(
	Notic,普通信息;
	Warning,警告信息;
	Err, 错误信息

	1)扩展新宏按照LOGXXX定义.
    2)定义后统一在void CLogManager2::Init()加入调用下
	3)只支持一层目录定义
*/
#define g_Log2 CLogManager2::getSingleton()

//业务逻辑
#define  LOGN(format, ...)  g_Log2.Write("log\\Notice", __FILE__, __LINE__, format, __VA_ARGS__)
#define  LOGW(format, ...)  g_Log2.Write("log\\Warning", __FILE__, __LINE__, format, __VA_ARGS__)
#define  LOGE(format, ...)  g_Log2.Write("log\\Err", __FILE__, __LINE__, format, __VA_ARGS__)

//网络模块
#define  LOGNE(format, ...)  g_Log2.Write("log\\ErrNet", __FILE__, __LINE__, format, __VA_ARGS__)
#define  LOGNN(format, ...)  g_Log2.Write("log\\NoticeNet", __FILE__, __LINE__, format, __VA_ARGS__)

//内存模块
#define  LOGME(format, ...)  g_Log2.Write("log\\ErrMemery", __FILE__, __LINE__, format, __VA_ARGS__)
#define  LOGMN(format, ...)  g_Log2.Write("log\\NoticeMemery", __FILE__, __LINE__, format, __VA_ARGS__)


#define  LOGU1(format, ...) g_Log2.Write("log\\UserDefine1", __FILE__, __LINE__, format, __VA_ARGS__)



namespace name_log2
{
	const int SAVE_FILE_TITLE_NAME_LEN = 32;   //保存的文件名长度(不包括时间)
	const int SAVE_FILE_NAME_LEN = SAVE_FILE_TITLE_NAME_LEN+30;   //总长度，30为时间+.log

	const int LINE_BUFFER_LEN = 32*1024;        //kb,单次允许写入临时缓冲区的大小
//#todo 看看能不能优化成多个日志使用一个队列
	//环形缓冲区大小,写入到终端前的临时存放空间, 要大于LINE_BUFFER_LEN
	const int CIRCLE_BUFFER_LEN = LINE_BUFFER_LEN*3;  

	/*
		输出终端的控制
	*/
	const int PUT_MASK_CONSOLE = 0x1;		//控制台
	const int PUT_MASK_HD = 0x10;			//磁盘

	const int PUT_MASK = PUT_MASK_CONSOLE|PUT_MASK_HD;
};


class CLog2
{
public:
	CLog2();
	~CLog2();

	bool Init(const char* pSaveFileName);
	void Write(LPCSTR pFile, int nLine, LPCSTR format, va_list VaList);
	void FlushWrite();

private:
	void InitCircleBuffer();
	void Release();

	//重建保存终端
	void ReCreateSaveTerminal();
	//建立终端
	bool CreateSaveTerminal();
	//关闭
	void CloseSaveTerminal();

	//输出到各个终端
	void WriteTerminal(LPCSTR pFile, int nLine, const char* format, va_list VaList);

public:

private:
	//终端句柄（现在只有硬盘）
	FILE* m_hFile;

	/*
		缓冲区
	*/
	//单次写入
	char m_szLineBuffer[name_log2::LINE_BUFFER_LEN];  

	//环形缓冲
	CCircleBuffer m_CircleBuffer;

	//文件名
	char m_szFileTitleName[name_log2::SAVE_FILE_TITLE_NAME_LEN];
	char m_szFileName[name_log2::SAVE_FILE_NAME_LEN ];

	//时间
	DWORD m_nFileNameHour;
};


class CLogManager2:public Singleton<CLogManager2>
{
public:
	CLogManager2();
	~CLogManager2();

	void Write(const char* pFileName,  LPCSTR pFile, int nLine, LPCSTR format, ...);

private:
	void Init();
	void StartThread();

	void Release();
	void StopThread();

	CLog2* FindLogInstance(const char* pFileName);
	CLog2* NewLogInstantce(const char* pFileName);

	static unsigned int WINAPI ThreadWrite(void* param);
public:

private:
	typedef std::map<std::string,CLog2*> MAP_LOG_INSTANCE;
	MAP_LOG_INSTANCE m_mapLogInstantce;

	volatile bool m_bThreadRun;
	HANDLE m_hThread;
	unsigned int m_idThread;
};










