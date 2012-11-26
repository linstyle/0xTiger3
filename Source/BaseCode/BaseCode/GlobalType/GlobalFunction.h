/*  作者：		林东平                                                              
	创建时间：	2011/3/8                                                       
    功能说明：  一些零散的全局函数,标准库IO操作封装等.量多了再细分
*/

#pragma once

#include <wchar.h>
#include <stdlib.h>
#include "MTASSERT.h"

//校验版MEMSET
#define MEMSET(DST, VAL, S)  {IF(sizeof(*DST)!=S){} memset(DST, VAL, S);}

//inet_addr宽字符版
unsigned long inet_addr_w(const wchar_t *pSrcIP);

//随机数
int Random(double start, double end);
