/*  作者：		林东平                                                              
	创建时间：	2011/2/21                                                       
    功能说明：  一个分等级的LEVEL。通过指定LEVEL的不同，
					   内容会写到不同的文件里。算是对CLOG的一个封装升级
*/
#pragma once

#include "Singleton.h"
#include "CLog.h"


//给外部直接调用的接口
void LOG(LOG_LEVEL Level, wchar_t* format, ...);

class CLogManager:public Singleton<CLogManager>
{
public:
	CLogManager();
	~CLogManager();

	void Init();
	void Write(LOG_LEVEL Level, wchar_t* format, va_list VaList);
private:

public:

private:
	CLog *m_pLog;
	CLogThread m_LogThread;
};

#define g_LogManager CLogManager::getSingleton()