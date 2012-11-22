#include "CLog2.0.h"
#include <windows.h>
#include <process.h>
#include "MTASSERT.h"
#include "mystdio.h"

initialiseSingleton(CLogManager2);

void CLogManager2::Init()
{
	/*
		对各个日志实例都初始化一遍,保证都创建
	*/
	LOGN("");
	LOGW("");
	LOGE("");

	LOGNN("");
	LOGNW("");
	LOGNE("");

	LOGMN("");
	LOGME("");
	LOGU1("");
	StartThread();
}

CLog2::CLog2()
{
	m_hFile = NULL;
	m_nFileNameHour = 0;
	memset(m_szFileName, 0, sizeof(m_szFileName));
	memset(m_szFileTitleName, 0, sizeof(m_szFileTitleName));
}

CLog2::~CLog2()
{
	Release();
}

bool CLog2::Init(const char* pSaveFileName)
{
	strncpy_s(m_szFileTitleName, pSaveFileName,  name_log2::SAVE_FILE_TITLE_NAME_LEN-1);

	InitCircleBuffer();

	//开文件
	return CreateSaveTerminal();
}

void CLog2::InitCircleBuffer()
{
	m_CircleBuffer.Init(name_log2::CIRCLE_BUFFER_LEN);
}

void CLog2::Release()
{
	CloseSaveTerminal();
}

void CLog2::Write(LPCSTR pFile, int nLine, LPCSTR format, va_list VaList)
{
	//检测是否要重建
	ReCreateSaveTerminal();

	//输出到各个终端设备
	WriteTerminal(pFile, nLine, format, VaList);
}

void CLog2::FlushWrite()
{
	if (NULL==m_hFile)
	{
		printf("Err, CLog2::FlushWrite(),NULL==m_hFile\n");
		return;
	}
	/*
		每次按36KB刷
	*/
	const int FLUSH_BYTES = 36*1024;
	unsigned int nFlushBytes = 0, nHasFlushBytes=0;

	int nCircleBufferReadLen=0;
	char *pTempBuffer = m_CircleBuffer.GetReadBuffer(&nCircleBufferReadLen);
	if (0==nCircleBufferReadLen)
	{
		return;
	}

	nFlushBytes = nCircleBufferReadLen>FLUSH_BYTES ? FLUSH_BYTES:nCircleBufferReadLen;
	
	while (nFlushBytes)
	{
		int nRet=fwrite(pTempBuffer, 1, nFlushBytes, m_hFile);
		if (0==nRet)
		{
			printf("Err, CLog::Flush():fwrite:ErrNum(%d)\n", GetLastError());
			m_CircleBuffer.ReadBufferFlush(nHasFlushBytes);
			
			return;
		}

		pTempBuffer+=nRet;
		nHasFlushBytes+=nRet;
		nFlushBytes-=nRet;
	}

	fflush(m_hFile);

	m_CircleBuffer.ReadBufferFlush(nHasFlushBytes);

	//#todo 如果不满足FLUSH_KB,可以再来一次，日后看下写入曲线是否平和

	return;	
}

//重建保存终端
void CLog2::ReCreateSaveTerminal()
{
	//时间,一小时记录一次.
	DWORD nNowHout = CTime::GetHH();
	if (nNowHout==m_nFileNameHour)
	{
		return;
	}
	//#todo 按体积大小判断

	CloseSaveTerminal();
	CreateSaveTerminal();
}

bool CLog2::CreateSaveTerminal()
{
	if (!m_hFile)
	{
		return false;
	}

	//取到文件名
	m_nFileNameHour = CTime::GetHH();
	_snprintf_s(m_szFileName, sizeof(m_szFileName)-1, "%s%d.log", m_szFileTitleName,
		CTime::GetMMDDHH() );

	//创建文件夹
	char *pPathNameFlag = strstr(m_szFileTitleName, "\\");
	if (pPathNameFlag)
	{
		char szPathName[32]={0};
		char *pszPathName = szPathName;
		char c=*pPathNameFlag;
		*pPathNameFlag='\0';

		strncpy_s(szPathName, m_szFileTitleName, 32);
		
		if ( !CreateDirectoryA(pszPathName, NULL) )
		{
			if (GetLastError()!=ERROR_ALREADY_EXISTS)
			{
				printf("Err,CLog2::CreateSaveTerminal(). ErrNum:%d\n", GetLastError());
				return false;
			}
		}

		*pPathNameFlag = c;
	}

	//创建文件
	if (0!=fopen_s(&m_hFile, m_szFileName, "a+"))
	{
		printf("Err, CLog2::CreateSaveTerminal():ErrID(%d)\n", GetLastError());
		return false;
	}
	
	return true;
}

void CLog2::CloseSaveTerminal()
{
	if(NULL!=m_hFile)
	{
		fflush(m_hFile);
		fclose(m_hFile);
		m_hFile = NULL;
	}	
}

//输出到各个终端
void CLog2::WriteTerminal(LPCSTR pFile, int nLine, const char* format, va_list VaList)
{
	MEMSET(&m_szLineBuffer, 0 , sizeof(m_szLineBuffer));
	TIME_HHMMSS TimeHHMMSS;
	int nSprintfLen=0, nSumSprintfLen=0, nPrefixLen=0;
	char *pszLineBuffer = m_szLineBuffer;

	CTime::GetHHMMSS(&TimeHHMMSS);

	//一些固定的前缀内容
	nSprintfLen = _snprintf(pszLineBuffer, name_log2::LINE_BUFFER_LEN, 
		"%s %s:%d ", TimeHHMMSS.m_Buffer, pFile, nLine);

	if (-1!=nSprintfLen)
	{
		nPrefixLen=nSprintfLen;
		nSumSprintfLen+=nSprintfLen;
	}
	//具体的内容
	nSprintfLen = _vsnprintf(pszLineBuffer+nSumSprintfLen, name_log2::LINE_BUFFER_LEN-nSumSprintfLen, format, VaList);
	if (-1!=nSprintfLen)
	{
		nSumSprintfLen+=nSprintfLen;
	}

	if (NULL==*(pszLineBuffer+nPrefixLen))
	{
		return;
	}

	if (name_log2::PUT_MASK & name_log2::PUT_MASK_CONSOLE)
	{
		printf("%s\n", pszLineBuffer+nPrefixLen);
	}

	if (name_log2::PUT_MASK & name_log2::PUT_MASK_HD)
	{
		if ( !m_CircleBuffer.WriteBufferAtom(m_szLineBuffer, nSumSprintfLen) )
		{
			LOGW("Warning, CLog2::WriteTerminal:m_CircleBuffer.WriteBuffer, nSumSprintfLen(%d)\n", nSumSprintfLen);
		}
	}
}

void CLogManager2::Write(const char* pFileName ,  LPCSTR pFile, int nLine, LPCSTR format, ...)
{
	va_list valist;

	va_start(valist, format);
	
	CLog2 *pLog = FindLogInstance(pFileName);
	if (NULL==pLog)
	{
		printf("Err, CLogManager2::Write(), NULL==pLog\n");
		return;
	}
	pLog->Write(pFile, nLine, format, valist);

	va_end(valist);	
}

CLogManager2::CLogManager2()
{
	m_bThreadRun = true;
	m_idThread = 0;
	m_hThread = NULL;
	Init();
}

CLogManager2::~CLogManager2()
{
	Release();
}

void CLogManager2::Release()
{
	StopThread();

	MAP_LOG_INSTANCE::iterator ite;
	
	for (ite = m_mapLogInstantce.begin(); ite!=m_mapLogInstantce.end(); ite++)
	{
		delete ite->second;
	}
}

CLog2* CLogManager2::FindLogInstance(const char* pFileName)
{
	MAP_LOG_INSTANCE::iterator ite = m_mapLogInstantce.find(pFileName);;

	if (ite==m_mapLogInstantce.end())
	{
		//新建
		return NewLogInstantce(pFileName);
	}

	return ite->second;
}

CLog2* CLogManager2::NewLogInstantce(const char* pFileName)
{
	CLog2 *pLog = new CLog2;

	if ( false==pLog->Init(pFileName) )
	{
		return NULL;
	}
	
	m_mapLogInstantce[pFileName]=pLog;

	return pLog;
}

void CLogManager2::StartThread()
{
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadWrite,(void *)this, 0, &m_idThread);
	INITASSERT(0==m_hThread);
}

void CLogManager2::StopThread()
{
	m_bThreadRun = false;

	if( WAIT_FAILED==WaitForSingleObject(m_hThread, 3000) )
	{
		printf("Err, CLogManager2::StopThread,ErrNum:%d\n", GetLastError());
	}
	CloseHandle(m_hThread);
}

unsigned int WINAPI CLogManager2::ThreadWrite(void* param)
{
	MAP_LOG_INSTANCE::iterator ite;

	CLogManager2 *pLogManager2 = (CLogManager2 *)param;

	while(pLogManager2->m_bThreadRun)
	{
		for (ite=pLogManager2->m_mapLogInstantce.begin(); ite!=pLogManager2->m_mapLogInstantce.end(); ite++)
		{
			CLog2 *pLog2 = ite->second;
			pLog2->FlushWrite();
		}

		Sleep(300);
	}

	return 0;
}