/*  作者：		林东平                                                              
	创建时间：	2010/06/17                                                         
    功能说明：  常用的类型、平台关键字段大小定义
*/
#pragma once

#include <stdio.h>
#include "MTASSERT.h"
#include "CLog2.0.h"
#include "CMyStackWalker.h"

typedef unsigned __int64 MGUID;
typedef INT64  OBJID;

typedef signed __int64 S64;
typedef signed __int32 S32;
typedef signed __int16 S16;
typedef signed __int8 S8;

typedef unsigned __int64 U64;

typedef unsigned __int16 U16;
typedef unsigned __int8 U8;




#define _ALIGN(a,size) (((a)+(size)-1)&(~((size)-1)))
const unsigned int  __PAGE_SIZE_BYTE = 4096;		 //byte
const unsigned int __ALIGN_SIZE_LONG = sizeof(long);		 //4字节对齐,long在大多数平台下和CPU位数相关
const unsigned int __ALIGN_SIZE_PAGE = __ALIGN_SIZE_LONG;     //这个应该有API得到吧？

#define printu(level) printf(level##",%s::%s:%d, %d\n", __FILE__, __FUNCTION__, __LINE__, GetLastError())

#define SAFE_FREE(p)	\
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



#define SAFE_DELETE(p)	\
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


#define SAFE_DELETE_ARRAY(p)	\
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