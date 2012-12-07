// BaseCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vld.h>
#include "CCommonLibInit.h"
#include "NetAPI.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CCommonLibInit CommonLibInit;

	//ÍøÂçµØÖ·
	//net::SetSocketServer("haha","0.0.0.0",9999);
	net::AddConnectSocket("0.0.0.0",9999);
	net::Init();

	for (int i=0; i<100; i++)
	{
		LOGU1("%d\n", i);
	}
	Sleep(3000);

	return 0;
}

