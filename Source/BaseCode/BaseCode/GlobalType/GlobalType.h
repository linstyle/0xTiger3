/*  ���ߣ�		�ֶ�ƽ                                                              
	����ʱ�䣺	2010/06/17                                                         
    ����˵����  ���õ����͡�ƽ̨�ؼ��ֶδ�С����
*/
#pragma once

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include "MTASSERT.h"
#include "CLog2.0.h"
#include "CMyStackWalker.h"


typedef UINT64 MGUID;

typedef INT64  OBJID;


#define _ALIGN(a,size) (((a)+(size)-1)&(~((size)-1)))
const unsigned int  __PAGE_SIZE_BYTE = 4096;		 //byte
const unsigned int __ALIGN_SIZE_LONG = sizeof(long);		 //4�ֽڶ���,long�ڴ����ƽ̨�º�CPUλ�����
const unsigned int __ALIGN_SIZE_PAGE = __ALIGN_SIZE_LONG;     //���Ӧ����API�õ��ɣ�

#define printu(level) printf(level##",%s::%s:%d, %d\n", __FILE__, __FUNCTION__, __LINE__, GetLastError())

#define FREE_NULL(p)	\
do									\
{										\
	__try								\
	{									\
		if(p)							\
			free(p);					\
		p=NULL;					\
	}									\
	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))		\
	{										\
		return;							\
	}										\
} while (0)



#define DELETE_NULL(p)	\
do										\
{											\
	__try									\
	{										\
		if(p)								\
			delete p;					\
		p=NULL;						\
	}										\
	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))		\
	{										\
		return;							\
	}										\
} while (0)


#define DELETE_ARRAY_NULL(p)	\
do										\
{											\
	__try									\
	{										\
		if(p)								\
			delete []p;				\
		p=NULL;						\
	}										\
	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))		\
	{										\
		return;							\
	}										\
} while (0)