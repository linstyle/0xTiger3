#include "StdAfx.h"
#include "CJavaRandom.h"


unsigned __int64 CJavaRandom::multiplier = 0x5DEECE66DL;
unsigned __int64 CJavaRandom::addend = 0xBL;
unsigned __int64 CJavaRandom::mask = (1L << 48) - 1;