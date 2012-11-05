/*
	@lindp
	@¶ÔStackWalker·â×°
	@2012/01/18
*/

#pragma  once

#include "StackWalker.h"
#include "CLog2.0.h"

class MyStackWalker : public StackWalker
{
public:
	MyStackWalker() : StackWalker() {}
	MyStackWalker(DWORD dwProcessId, HANDLE hProcess) : StackWalker(dwProcessId, hProcess) {}
	virtual void OnOutput(LPCSTR szText) { printf(szText); StackWalker::OnOutput(szText); LOGE(szText);}
};

LONG WINAPI ExpFilter(EXCEPTION_POINTERS* pExp, DWORD dwExpCode);

#define  TRACER_STACK()  MyStackWalker sw;sw.ShowCallstack()