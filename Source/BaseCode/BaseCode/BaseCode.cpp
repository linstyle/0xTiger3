// BaseCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vld.h>
#include "CCommonLibInit.h"

int _tmain(int argc, _TCHAR* argv[])
{
	new CCommonLibInit;
	Sleep(10000);

	delete CCommonLibInit::getSingletonPtr();
	return 0;
}

