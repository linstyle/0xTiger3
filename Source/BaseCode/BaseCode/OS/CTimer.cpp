#include "CTimer.h"
#include "mystdio.h"

CTimer::CTimer()
{
	Init();
}

void CTimer::Init()
{
	InitVecs();
	InitTime();
}

void CTimer::InitVecs()
{
	MEMSET(m_Count, 0, sizeof(m_Count));
	/*
		初始化好vec的指针
	*/
	m_Tvecs[0] = (TIMER_VEC*)&m_TvRoot;
	for(int i=0; i<name_timer::VEC_SIZE; ++i)
	{
		m_Tvecs[i+1] = &m_Tv[i];
	}
}

void CTimer::InitTime()
{
	UpdateEndTime();
	UpdateBeginTime( GetEndTime() );
}

void CTimer::RunTimerList()
{
	UpdateEndTime();

	if( false==TimeAfterEq(GetEndTime(), GetBeginTime()) )
	{
		return;
	}

	__RunTimer();
}

bool CTimer::AddTimer(TIMER_ARG *pTimer)
{
	if( pTimer->IntervalTime > 0xffffffffUL )
	{
		printf("Err, CTimer::AddTimer, too long\n");
		return false;		
	}

	__AddTimer(pTimer);

	return true;
}

bool CTimer::DelTimer(TIMER_ARG *pTimer)
{
	__DelTimer(pTimer);
	return true;
}

void CTimer::__DelTimer(TIMER_ARG *pTimer)
{
	list_del(&pTimer->ListNext);
}

void CTimer::__AddTimer(TIMER_ARG *pTimer)
{
	timer_t nExpires = pTimer->IntervalTime;
	timer_t nIdx = nExpires - GetBeginTime();
	struct list_head *pVec;

	/*
		0x0 <=interval <0xff
		0x100≤interval≤0x3fff			1>>8+6
		0x4000≤interval≤0xfffff       1>>8+6+6
		0x100000≤interval≤0x3ffffff   1>>8+6+6+6
		0x4000000≤interval≤0xffffffff 1>>8+6+6+6+6
	*/
	int i, nX=0, nY=0;
	if (nIdx < name_timer::TVR_SIZE) //0x0 <=interval <0xff
	{
		i = nExpires & name_timer::TVR_MASK;
		pVec = m_Tvecs[0]->Vec + i;
		nX=0;
		nY=i;
	} 
	else if (nIdx < 1 << (name_timer::TVR_BITS + name_timer::TVN_BITS)) //0x3fff
	{
		i = (nExpires >> name_timer::TVR_BITS) & name_timer::TVN_MASK;
		pVec = m_Tvecs[1]->Vec + i;
		nX=1;
		nY=i;
		
	} 
	else if (nIdx < 1 << (name_timer::TVR_BITS + 2 * name_timer::TVN_BITS)) //0xfffff
	{
		i = (nExpires >> (name_timer::TVR_BITS + name_timer::TVN_BITS) ) & name_timer::TVN_MASK;
		pVec = m_Tvecs[2]->Vec + i;
		nX=2;
		nY=i;
	} 
	else if (nIdx < 1 << (name_timer::TVR_BITS + 3 * name_timer::TVN_BITS)) 
	{
		i = (nExpires >> (name_timer::TVR_BITS + name_timer::TVN_BITS*2) )& name_timer::TVN_MASK;
		pVec = m_Tvecs[3]->Vec + i;
		nX=3;
		nY=i;
	} 
	else
	{
		i = (nExpires >> (name_timer::TVR_BITS + name_timer::TVN_BITS*3) ) & name_timer::TVN_MASK;
		pVec = m_Tvecs[4]->Vec + i;	
		nX=4;
		nY=i;
	}

	printf("__AddTimer, x:%d, y:%d, time:%d\n", nX, nY, nExpires);
	/*
		FIFO
	*/
	list_add_tail(&pTimer->ListNext, pVec);
	m_Count[nX][nY]++;
}

void CTimer::__RunTimer()
{
	list_head ListWork;
	list_head *pRunListHead = &ListWork;

	while( true==TimeAfterEq(GetEndTime(), GetBeginTime()) )
	{
		UpdateTv();

		list_replace_init(m_TvRoot.Vec+m_TvRoot.nIndex, &ListWork);
		RunCallBackList(pRunListHead);
	
		timer_t t = GetBeginTime();
		t++;
		UpdateBeginTime( t );
	}
}

void CTimer::UpdateTv()
{
	int nIndex = GetBeginTime() & name_timer::TVR_MASK;
	if( !nIndex )
	{
		for(int i=0; i<name_timer::VEC_SIZE; ++i)
		{
			if( Cascade(&m_Tv[i], CascadeIndex(i)) )
			{
				break;
			}
		}
	}
	m_TvRoot.nIndex= nIndex;
}

int CTimer::Cascade(TIMER_VEC *pTimerVec, int nIndex)
{
	TIMER_ARG *pTimerArg;
	list_head ListTv, *pEach;

	list_replace_init(pTimerVec->Vec+nIndex, &ListTv);

	while( NULL != (pEach=list_get(&ListTv)) )
	{
		pTimerArg = list_entry_offset(pEach, TIMER_ARG, ListNext);
		__AddTimer(pTimerArg);		
	}
	
	return nIndex;
}

void CTimer::RunCallBackList(list_head *pHead)
{
	TIMER_ARG *pTimerArg;
	list_head *pEach;

	/*
		取出同时，已经从链表删除
	*/
	while( NULL!= (pEach=list_get(pHead)) )
	{
		pTimerArg = list_entry_offset(pEach, TIMER_ARG, ListNext);

		void (*fn)(TIMER_ARG* pTimerArg);

		fn = pTimerArg->CallBackFn;

		fn(pTimerArg);
	}

}

void CTimer::TracerMe()
{
	printf("------notic, CTimer::TracerMe()\n");
	int i,j;

	printf("tvroot:\n");
	for(i=0; i<name_timer::TVR_SIZE; ++i)
	{
		printf("[%d,%d]:%d ", 0, i, m_Count[0][i]);
		if( i%8==0 )
		{
			printf("\n");
		}
	}

	printf("\ntv:\n");
	for(i=0; i<name_timer::VEC_SIZE; ++i)
	{
		for(j=0; j<name_timer::TVN_SIZE; ++j)
		{
			printf("[%d,%d]:%d ", i+1, j, m_Count[i+1][j]);
			if( j%8==0 )
			{
				printf("\n");
			}
		}
	}
	printf("\n-----end notic, CTimer::TracerMe()\n");
}