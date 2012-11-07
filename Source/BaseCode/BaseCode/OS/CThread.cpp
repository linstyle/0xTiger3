#include "StdAfx.h"
#include "CThread.h"

CThread::CThread()
{
	m_hThread = NULL;
	m_uThread = 0;
}

CThread::~CThread()
{
	CloseHandle(m_hThread);
}

DWORD CThread::Suspend()
{
	return SuspendThread(m_hThread);
}
DWORD CThread::Resume()
{
	return ResumeThread(m_hThread);
}
DWORD CThread::Join()
{
	return WaitForSingleObject(m_hThread, INFINITE);
}