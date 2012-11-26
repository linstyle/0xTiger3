/*
	@lindp lin_style@foxmail.com   
	@2012/3/12
	循环从网络层中取到包
*/

#pragma once
#include "Singleton.h"

class CLogicDriver:public Singleton<CLogicDriver>
{
public:
	void Loop();

private:

public:

private:

};

#define  g_LogicDriver CLogicDriver::getSingleton()