/*  ���ߣ�		�ֶ�ƽ                                                              
	����ʱ�䣺	2011/3/8                                                       
    ����˵����  һЩ��ɢ��ȫ�ֺ���,��׼��IO������װ��.��������ϸ��
*/

#pragma once

#include <wchar.h>
#include <stdlib.h>
#include "MTASSERT.h"

//У���MEMSET
#define MEMSET(DST, VAL, S)  {IF(sizeof(*DST)!=S){} memset(DST, VAL, S);}

//inet_addr���ַ���
unsigned long inet_addr_w(const wchar_t *pSrcIP);

//�����
int Random(double start, double end);
