/*  作者：		林东平                                                              
	创建时间：	2010/06/21                                                         
    功能说明：  初始化等即时调试和静态检测
	更新：2012/01/13 
*/

#pragma once
#pragma comment( lib, "USER32" )
#include <windows.h>

#include "CLog2.0.h"

/*
	感谢飞雪提供代码,一个静态的宏检测
*/
#ifdef	HAS_STATIC_ASSERT
	#define	STATIC_ASSERT(x) static_assert(x, #x)
#else
	namespace
	{
		template <bool x>
			struct STATIC_ASSERTION_FAILURE;
		template <>
			struct STATIC_ASSERTION_FAILURE<true> { enum { value = 1 }; };
		template<int x>
			struct static_assert_test{};
	}
	#define STATIC_ASSERT(x) typedef static_assert_test<\
									sizeof(STATIC_ASSERTION_FAILURE<(bool)(x) >)\
									>	static_assert_typedef
#endif


#ifdef _X86_
#define DebugBreak()    _asm { int 3 }
#endif

__inline void CmAssert(LPCSTR pFile, int nLine, PCSTR pExpr, int nErrnum)
{
	char szMsg[2*MAX_PATH];
	wsprintfA(szMsg, "File %s, line %d : %s, errnum:%d", pFile, nLine, pExpr, nErrnum);

	char szTitle[MAX_PATH];
	GetModuleFileNameA(NULL, szTitle, _countof(szTitle));
	MessageBoxA(GetActiveWindow(), szMsg, szTitle, MB_OK);
	DebugBreak();
}

/*
	静态初始化(程序启动，资源初始化必须对的)使用的判定宏。
*/
#define  INITASSERT(a) if ((a)) InitAssert(__FILE__, __LINE__, #a, GetLastError())
__inline void InitAssert(LPCSTR pFile, int nLine, PCSTR pExpr, int nErrnum)
{
	LOGE("Err, InitAssert. Expr:%s, ErrNum:%d\n", pExpr, nErrnum);
	CmAssert(pFile, nLine, pExpr, nErrnum);
}



/*
	程序运行时的判定宏。DEBUG版本下会弹框+日志，Release下只会记录日志
	MTASSERT定义为宏是因为LOG中会记录__FILE__信息，两者冲突
*/

#ifdef _DEBUG
	#define  MTASSERT(pFile, nLine, pExpr, nErrnum)	\
	LOGE("Err, MTASSERT. Expr:%s, ErrNum:%d\n", pExpr, nErrnum)	,		\
	CmAssert(pFile, nLine, pExpr, nErrnum)		
#else
	#define  MTASSERT(pFile, nLine, pExpr, nErrnum) \
		LOGE("Err, InitAssert. Expr:%s, ErrNum:%d\n", pExpr, nErrnum),
#endif


#define  IF(a) if( (a) ?(MTASSERT(__FILE__, __LINE__, #a, GetLastError()),1) :  0)

/*
	跟MTASSERT一样，只是给网络层使用，换个文件名
	只是IFn这个宏名字难记了点，小n表示net
*/
#ifdef _DEBUG
#define  MTASSERTn(pFile, nLine, pExpr, nErrnum)	\
	LOGNE("Err, MTASSERT. Expr:%s, ErrNum:%d\n", pExpr, nErrnum),		\
	CmAssert(pFile, nLine, pExpr, nErrnum)
#else
#define  MTASSERT(pFile, nLine, pExpr, nErrnum) \
	LOGNE("Err, InitAssert. Expr:%s, ErrNum:%d\n", pExpr, nErrnum),
#endif


#define  IFn(a) if( (a) ?(MTASSERTn(__FILE__, __LINE__, #a, GetLastError()),1) :  0)

/*
	跟MTASSERT一样，只是给内存池(slab)层使用，换个文件名
	只是IFm这个宏名字难记了点，小n表示memry	
*/
#ifdef _DEBUG
#define  MTASSERTm(pFile, nLine, pExpr, nErrnum)	\
	LOGME("Err, MTASSERT. Expr:%s, ErrNum:%d\n", pExpr, nErrnum)	,		\
	CmAssert(pFile, nLine, pExpr, nErrnum)
#else
#define  MTASSERTm(pFile, nLine, pExpr, nErrnum) \
	LOGME("Err, InitAssert. Expr:%s, ErrNum:%d\n", pExpr, nErrnum),
#endif


#define  IFm(a) if( (a) ?(MTASSERTm(__FILE__, __LINE__, #a, GetLastError()),1) :  0)