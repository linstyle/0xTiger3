#include "StdAfx.h"
#include "CThreadQuit.h"
#include <process.h>

initialiseSingleton(CThreadQuit);

CThreadQuit::CThreadQuit()
{
	Init();
}

CThreadQuit::~CThreadQuit()
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
}

void CThreadQuit::Init()
{
	//Æô¶¯Ïß³Ì
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, 0, 0, &m_uThread);	
	INITASSERT( 0== m_hThread);
}

unsigned int WINAPI CThreadQuit::ThreadProc(void* pParam)
{
	GetQuitCommand();
	return 0;
}
