/************************************************************************/
/*  作者：		林东平                                                              
	创建时间：	2010/06/17                                                         
    功能说明：  锁文件。
				分为临界区.CLock和
				递归锁     CFastLock(不持有)
*/
/************************************************************************/

#pragma once
#include <Windows.h>

class CLock
{
public:
	CLock();
	~CLock();

	void Lock()
	{
		EnterCriticalSection(&cs);
	}
	void UnLock()
	{
		LeaveCriticalSection(&cs);
	}
	bool TryLock()
	{
		return (TryEnterCriticalSection(&cs) == TRUE ? true : false);
	}

private:
	
public:

private:
	CRITICAL_SECTION cs;
};


class CFastLock
{
public:
	CFastLock();

	enum
	{
		FASTLOCK_LOCK = 1,
		FASTLOCK_UNLOCK = 0
	};

	void Lock()
	{
		LONG own;

		for(;;)
		{
			own=InterlockedCompareExchange(&m_lLock, FASTLOCK_LOCK, FASTLOCK_UNLOCK);
			if( FASTLOCK_UNLOCK==own )
			{
				break;
			}				
			
			Sleep(0);
		}
	}

	void UnLock()
	{
		InterlockedExchange(&m_lLock, FASTLOCK_UNLOCK);
	}

	bool TryLock()
	{
		LONG own;

		own = InterlockedCompareExchange(&m_lLock, FASTLOCK_LOCK, FASTLOCK_UNLOCK);
		if( FASTLOCK_UNLOCK==own )
		{
			return true;
		}

		return false;
	}

private:

public:

private:
	volatile LONG m_lLock;	
};

/*
	原子变量操作
*/
class CAtom
{
	volatile LONG m_t;

public:
	CAtom()
	{
	
	}
	CAtom(int n)
	{
		InterlockedExchange(&m_t, n);
	}
	LONG GetValue()
	{
		return m_t;
	}
	/*
		=
	*/
	LONG operator=(int n)
	{
		InterlockedExchange(&m_t, n);
		return m_t;
	}

	LONG operator=(CAtom c)
	{
		InterlockedExchange(&m_t, c.m_t);
		return m_t;
	}

	/*
		后增量
	
    LONG operator++(int)
	{
		return InterlockedCompareExchange(&m_t, m_t+1, m_t);
	}

    LONG operator--(int)
	{
		return InterlockedCompareExchange(&m_t, m_t-1, m_t);
	}
*/
	/*
		前增量
	*/
	LONG operator++()
	{
		InterlockedIncrement(&m_t);
		return m_t;
	}
	LONG operator--()
	{
		InterlockedDecrement(&m_t);
		return m_t;
	}

	/*
		+
	*/
	LONG operator+(LONG nAdd)
	{		
		InterlockedExchangeAdd(&m_t, nAdd);
		return m_t;
	}
	/*
		-
	*/
	LONG operator-(LONG nAdd)
	{		
		InterlockedExchangeAdd(&m_t, -nAdd);
		return m_t;
	}
};
