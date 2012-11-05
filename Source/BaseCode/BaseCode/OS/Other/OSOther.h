/*  作者：		林东平                                                              
	创建时间：	2010/06/17                                                         
    功能说明：  需要涉及到操作系统特写零碎的代码
*/

#pragma once
#include "GlobalType.h"
#include  <conio.h>


/*
	函数功能:截取ctrl+c的退出代码
*/
void GetQuitCommand();

/*
	函数功能:获取CPU核数
*/
int GetCPUNumberOf();

/*
	函数功能:返回CPU当前运行的核数编号
*/
int GetCPURunId();

