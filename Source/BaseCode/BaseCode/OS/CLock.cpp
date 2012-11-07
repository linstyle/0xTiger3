
#include "CLock.h"

CLock::CLock()
{
	InitializeCriticalSection(&cs);
}

CLock::~CLock()
{
	DeleteCriticalSection(&cs);
}



CFastLock::CFastLock():m_lLock(FASTLOCK_UNLOCK)
{
	
}


/* Ê¹ÓÃ·¶Àý

CFastLock g_FastLock;

g_FastLock.Lock();

g_FastLock.UnLock();

*/