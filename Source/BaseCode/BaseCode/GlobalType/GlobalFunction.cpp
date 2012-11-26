#include "GlobalFunction.h"

unsigned long inet_addr_w(const wchar_t *pSrcIP)
{
	char  CharIP[128]={0};
	int   nSrcIPLen=0;

	nSrcIPLen=wcslen(pSrcIP);

	for(int i=0; i<nSrcIPLen; i++)
	{
		CharIP[i] = 0xFF&((char)pSrcIP[i]);
	}
	return inet_addr(CharIP);
}

int Random(double start, double end)
{
	return (int)(start+(end-start)*rand()/(RAND_MAX + 1.0));
}