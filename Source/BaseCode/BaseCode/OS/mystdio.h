/*  ���ߣ�		�ֶ�ƽ                                                              
	����ʱ�䣺	2012/11/15                                                         
    ����˵����  һЩ��׼���IO������װ
*/
#pragma once
#include "MTASSERT.h"

#define MEMSET(DST, VAL, S)  {IF(sizeof(*DST)!=S){} memset(DST, VAL, S);}

//char* SafeCopy(char* pDst, const char* pSrc, int nSrcLen)