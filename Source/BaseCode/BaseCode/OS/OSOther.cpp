#include "StdAfx.h"
#include "OSOther.h"
#include <time.h>
#include <stdio.h>
extern volatile bool g_bQuit;

void GetQuitCommand()
{
	while(1)  
	{  
		if(!_kbhit())  
		/*   Use   _getch   to   throw   key   away.   */  
		{  
			int c;
			if   ((c=_getch())==0x3)     //crrl   -c   
			{								
				g_bQuit = true;
				return;
			}
		}  
	}  
}

int GetCPUNumberOf()
{
	SYSTEM_INFO sinf;
	GetSystemInfo(&sinf);

	int nCPUNumberOf=0;

	nCPUNumberOf = sinf.dwNumberOfProcessors;

	return nCPUNumberOf;
}

int GetCPURunId()
{
    _asm {mov eax, 1} 
    _asm {cpuid} 
    _asm {shr ebx, 24} 
    _asm {mov eax, ebx} 
}
