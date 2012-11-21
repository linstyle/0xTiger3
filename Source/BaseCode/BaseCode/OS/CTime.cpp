
#include "CTime.h"

unsigned long CTime::GetYYMMDDHHMM()
{
	__time64_t szClock;
	struct tm TM;

	_time64( &szClock );
	localtime_s(&TM,  &szClock );

	return TM.tm_year%100*100000000 + 
			   (TM.tm_mon+1)* 1000000+
			   TM.tm_mday*      10000+
			   TM.tm_hour*        100+
			   TM.tm_min;
}

unsigned long CTime::GetMMDDHH()
{
	__time64_t szClock;
	struct tm TM;

	_time64( &szClock );
	localtime_s(&TM,  &szClock );

	return   (TM.tm_mon+1)* 10000+
				TM.tm_mday*       100+
				TM.tm_hour;
}

TIMEW_HHMMSS* CTime::GetWHHMMSS(TIMEW_HHMMSS *pTimeWHHMMSS)
{
	__time64_t szClock;
	struct tm TM;

	_time64( &szClock );
	localtime_s(&TM,  &szClock );

	swprintf(pTimeWHHMMSS->m_Buffer, sizeof(pTimeWHHMMSS->m_Buffer)-4,
		L"%02d:%02d:%02d",  TM.tm_hour, TM.tm_min, TM.tm_sec);

	return pTimeWHHMMSS;
}

TIME_HHMMSS* CTime::GetHHMMSS(TIME_HHMMSS *pTimeHHMMSS)
{
	__time64_t szClock;
	struct tm TM;

	_time64( &szClock );
	localtime_s(&TM,  &szClock );

	_snprintf_s(pTimeHHMMSS->m_Buffer, sizeof(pTimeHHMMSS->m_Buffer)-4,
		"%02d:%02d:%02d",  TM.tm_hour, TM.tm_min, TM.tm_sec);
	
	return pTimeHHMMSS;
}

unsigned long CTime::GetHH()
{
	__time64_t szClock;
	struct tm TM;

	_time64( &szClock );
	localtime_s(&TM,  &szClock );

	return TM.tm_hour;
}

__time64_t CTime::GetSS()
{
	__time64_t szClock;

	_time64( &szClock );

	return szClock;
}

__time64_t CTime::GetMilli()
{
	__time64_t szClock;
	struct _timeb TimeBuffer;

	_time64( &szClock );

	_ftime64_s(&TimeBuffer);

	return szClock*1000 + TimeBuffer.millitm;	
}