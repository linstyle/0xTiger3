/*  作者：		林东平                                                              
	创建时间：	2011/3/8                                                       
    功能说明：  一些零散的全局函数
*/

#pragma once
#include "GlobalType.h"
/*
	inet_addr宽字符版
	来源网上
*/
#include <wchar.h>

unsigned long inet_addr_w(const wchar_t *pSrcIP);
