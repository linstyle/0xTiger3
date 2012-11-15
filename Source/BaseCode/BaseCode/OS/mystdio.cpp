#include "mystdio.h"
#include "CMyStackWalker.h"

//char* SafeCopy(char* pDst, const char* pSrc, int nSrcLen)
//{
//	/*
//		主要检测pSrc长度和 nSrcLen
//	*/
//	IF(!pDst || !pSrc)
//		return -1;
//
//	__try
//	{
//		if (nSrcLen && strlen(pSrc) >= nSrcLen)
//		{
//			_strncpy(pDst, pSrc, nSrcLen-1);
//			pSrc[nSrcLen-1]=0;
//			return pDst;
//		}
//
//		return strcpy(pDst, pSrc);
//		
//	}
//	__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))	
//	{
//		return;
//	}	
//}