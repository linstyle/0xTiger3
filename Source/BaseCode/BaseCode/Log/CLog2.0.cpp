#include "CLog2.0.h"
#include <windows.h>
#include <process.h>
#include "MTASSERT.h"
#include "mystdio.h"

initialiseSingleton(CLogManager2);

CLog2::CLog2()
{
	m_hFile = NULL;
	m_nFileNameHour = 0;
	memset(m_szFileName, 0, sizeof(m_szFileName));
}

CLog2::~CLog2()
{
	Release();
}

bool CLog2::Init(const char* pSaveFileName)
{
	if (!pSaveFileName)
	{
		return false;
	}

	strncpy_s(m_szFileName, pSaveFileName,  sizeof(m_szFileName)-1);

	m_CircleBuffer.Init(name_log2::CIRCLE_BUFFER_LEN);

	//开文件
	return CreateSaveTerminal();
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
		__LOG("Err, CLog2::FlushWrite(),NULL==m_hFile\n");
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
			__LOG("Err, CLog::Flush():fwrite:ErrNum(%d)\n", GetLastError());
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
	//有文件？
	if (m_hFile)
	{
		return false;
	}

	//取到文件名
	m_nFileNameHour = CTime::GetHH();
	char szDynaFileName[name_log2::SAVE_LOG_NAME_LEN]={0};

	_snprintf_s(szDynaFileName, sizeof(szDynaFileName)-1, "%s%d.log", m_szFileName,
		CTime::GetMMDDHH() );

	//创建文件夹
	char *pHasDirectory = strstr(m_szFileName, "\\");	
	if (pHasDirectory)
	{
		//目录名
		char szDirecName[name_log2::SAVE_LOG_NAME_LEN/2]={0};
		sscanf(szDynaFileName, "%[^\\]", szDirecName);
		
		if ( !CreateDirectoryA(szDirecName, NULL) )
		{
			if (GetLastError()!=ERROR_ALREADY_EXISTS)
			{
				__LOG("Err,CLog2::CreateSaveTerminal(). ErrNum:%d\n", GetLastError());
				return false;
			}
		}
	}

	//创建文件
	m_hFile = fopen(szDynaFileName, "a+");
	if( NULL == m_hFile )
	{
		__LOG("Err, CLog2::CreateSaveTerminal():ErrID(%d)\n", GetLastError());
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
		printf("%s", pszLineBuffer+nPrefixLen);
	}

	if (name_log2::PUT_MASK & name_log2::PUT_MASK_HD)
	{
		if ( !m_CircleBuffer.WriteBufferAtom(m_szLineBuffer, nSumSprintfLen) )
		{
			__LOG("Warning, CLog2::WriteTerminal:m_CircleBuffer.WriteBuffer, nSumSprintfLen(%d)\n", nSumSprintfLen);
		}
	}
}

bool CLogManager2::Write(const char* pFileName ,  LPCSTR pFile, int nLine, LPCSTR format, ...)
{
	if (!pFileName)
	{
		return false;
	}

	va_list valist;

	va_start(valist, format);
	
	CLog2 *pLog = FindLogInstance(pFileName);
	if (NULL==pLog)
	{
		__LOG("Err, CLogManager2::Write(), NULL==pLog.FileName:%s\n", pFileName);
		return false;
	}
	pLog->Write(pFile, nLine, format, valist);

	va_end(valist);	

	return true;
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

void CLogManager2::Init()
{
	INITASSERT(!__LOG(""));

	StartThread();
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

	//先放入，防止INIT失败，内存泄露
	m_mapLogInstantce[pFileName]=pLog;

	if ( false==pLog->Init(pFileName) )
	{
		return NULL;
	}
	
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
		__LOG("Err, CLogManager2::StopThread,ErrNum:%d\n", GetLastError());
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