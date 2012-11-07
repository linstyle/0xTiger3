/*  作者：		林东平                                                              
	创建时间：	2011/3/8                                                       
    功能说明：  一些零散的全局函数
*/

#pragma once
#pragma comment( lib, "ws2_32.lib"  )

#include <wchar.h>
#include <stdlib.h>

//inet_addr宽字符版
unsigned long inet_addr_w(const wchar_t *pSrcIP);

//随机数
int Random(double start, double end);