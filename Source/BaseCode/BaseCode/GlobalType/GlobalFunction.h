/*  ���ߣ�		�ֶ�ƽ                                                              
	����ʱ�䣺	2011/3/8                                                       
    ����˵����  һЩ��ɢ��ȫ�ֺ���
*/

#pragma once
#pragma comment( lib, "ws2_32.lib"  )

#include <wchar.h>
#include <stdlib.h>

//inet_addr���ַ���
unsigned long inet_addr_w(const wchar_t *pSrcIP);

//�����
int Random(double start, double end);