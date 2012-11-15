/*  作者：		林东平                                                              
	创建时间：	2012/11/15                                                         
    功能说明：  一些标准库的IO操作封装
*/
#pragma once
#include "MTASSERT.h"

#define MEMSET(DST, VAL, S)  {IF(sizeof(*DST)!=S){} memset(DST, VAL, S);}

//char* SafeCopy(char* pDst, const char* pSrc, int nSrcLen)