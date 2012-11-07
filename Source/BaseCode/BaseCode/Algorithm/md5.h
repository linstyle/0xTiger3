/********************************************************************
	created:	2010/07/20
	created:	20:7:2010   14:34
	filename: 	d:\work\TemplateSln\TemplateSln\md5.h
	file path:	d:\work\TemplateSln\TemplateSln
	file base:	md5
	file ext:	h
	author:		chenfuyuan
	
	purpose:	MD5算法
*********************************************************************/
#pragma once

#include <Windows.h>
#include <assert.h>
#include <string>

typedef unsigned __int32 U32;

static const U32 MD5_SIZE = 16;

//Magic initialization constants
static const U32 MD5_INIT_STATE_0 = 0x67452301;
static const U32 MD5_INIT_STATE_1 = 0xefcdab89;
static const U32 MD5_INIT_STATE_2 = 0x98badcfe;
static const U32 MD5_INIT_STATE_3 = 0x10325476;

//Constants for Transform routine.
static const U32 MD5_S11 = 7;
static const U32 MD5_S12 = 12;
static const U32 MD5_S13 = 17;
static const U32 MD5_S14 = 22;
static const U32 MD5_S21 = 5;
static const U32 MD5_S22 = 9;
static const U32 MD5_S23 = 14;
static const U32 MD5_S24 = 20;
static const U32 MD5_S31 = 4;
static const U32 MD5_S32 = 11;
static const U32 MD5_S33 = 16;
static const U32 MD5_S34 = 23;
static const U32 MD5_S41 = 6;
static const U32 MD5_S42 = 10;
static const U32 MD5_S43 = 15;
static const U32 MD5_S44 = 21;

//Transformation Constants - Round 1
static const U32 MD5_T01 = 0xd76aa478; //Transformation Constant 1 
static const U32 MD5_T02 = 0xe8c7b756; // 2
static const U32 MD5_T03 = 0x242070db; // 3
static const U32 MD5_T04 = 0xc1bdceee; // 4
static const U32 MD5_T05 = 0xf57c0faf; // 5
static const U32 MD5_T06 = 0x4787c62a; // 6
static const U32 MD5_T07 = 0xa8304613; // 7
static const U32 MD5_T08 = 0xfd469501; // 8
static const U32 MD5_T09 = 0x698098d8; // 9
static const U32 MD5_T10 = 0x8b44f7af; // 10
static const U32 MD5_T11 = 0xffff5bb1; // 11
static const U32 MD5_T12 = 0x895cd7be; // 12
static const U32 MD5_T13 = 0x6b901122; // 13
static const U32 MD5_T14 = 0xfd987193; // 14
static const U32 MD5_T15 = 0xa679438e; // 15
static const U32 MD5_T16 = 0x49b40821; // 16

//s - Round 2
static const U32 MD5_T17 = 0xf61e2562; // 17
static const U32 MD5_T18 = 0xc040b340; // 18
static const U32 MD5_T19 = 0x265e5a51; // 19
static const U32 MD5_T20 = 0xe9b6c7aa;// 20
static const U32 MD5_T21 = 0xd62f105d; // 21
static const U32 MD5_T22 = 0x02441453; // 22
static const U32 MD5_T23 = 0xd8a1e681; // 23
static const U32 MD5_T24 = 0xe7d3fbc8; // 24
static const U32 MD5_T25 = 0x21e1cde6; // 25
static const U32 MD5_T26 = 0xc33707d6; // 26
static const U32 MD5_T27 = 0xf4d50d87; // 27
static const U32 MD5_T28 = 0x455a14ed; // 28
static const U32 MD5_T29 = 0xa9e3e905; // 29
static const U32 MD5_T30 = 0xfcefa3f8; // 30
static const U32 MD5_T31 = 0x676f02d9; // 31
static const U32 MD5_T32 = 0x8d2a4c8a; // 32

//s - Round 3
static const U32 MD5_T33 = 0xfffa3942; // 33
static const U32 MD5_T34 = 0x8771f681; // 34
static const U32 MD5_T35 = 0x6d9d6122; // 35
static const U32 MD5_T36 = 0xfde5380c; // 36
static const U32 MD5_T37 = 0xa4beea44; // 37
static const U32 MD5_T38 = 0x4bdecfa9; // 38
static const U32 MD5_T39 = 0xf6bb4b60; // 39
static const U32 MD5_T40 = 0xbebfbc70; // 40
static const U32 MD5_T41 = 0x289b7ec6; // 41
static const U32 MD5_T42 = 0xeaa127fa; // 42
static const U32 MD5_T43 = 0xd4ef3085; // 43
static const U32 MD5_T44 = 0x04881d05; // 44
static const U32 MD5_T45 = 0xd9d4d039; // 45
static const U32 MD5_T46 = 0xe6db99e5; // 46
static const U32 MD5_T47 = 0x1fa27cf8; // 47
static const U32 MD5_T48 = 0xc4ac5665; // 48

//s - Round 4
static const U32 MD5_T49 = 0xf4292244; // 49
static const U32 MD5_T50 = 0x432aff97; // 50
static const U32 MD5_T51 = 0xab9423a7; // 51
static const U32 MD5_T52 = 0xfc93a039; // 52
static const U32 MD5_T53 = 0x655b59c3; // 53
static const U32 MD5_T54 = 0x8f0ccc92; // 54
static const U32 MD5_T55 = 0xffeff47d; // 55
static const U32 MD5_T56 = 0x85845dd1; // 56
static const U32 MD5_T57 = 0x6fa87e4f; // 57
static const U32 MD5_T58 = 0xfe2ce6e0; // 58
static const U32 MD5_T59 = 0xa3014314; // 59
static const U32 MD5_T60 = 0x4e0811a1; // 60
static const U32 MD5_T61 = 0xf7537e82; // 61
static const U32 MD5_T62 = 0xbd3af235; // 62
static const U32 MD5_T63 = 0x2ad7d2bb; // 63
static const U32 MD5_T64 = 0xeb86d391; // 64


//Null data (except for first BYTE) used to finalise the checksum calculation
static unsigned char PADDING[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

class CMD5
{
public:
	/************************************************************************/
	/* 函数功能：计算所给字符串的MD5值
	   参数说明：pBuf --- 字符串指针
	             nLenght --- 字符串长度(strlen(pBuf))
	   返回值：字符串MD5值
	*/
	/************************************************************************/
	static std::string MD5(char* pBuf, UINT nLength);

private:
	CMD5();
	virtual ~CMD5() {};

	void Transform(BYTE Block[64]);
	void Update(BYTE* Input, ULONG nInputLen);
	std::string Final();
	inline DWORD RotateLeft(DWORD x, int n);
	inline void FF( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	inline void GG( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	inline void HH( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	inline void II( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	void DWordToByte(BYTE* Output, DWORD* Input, UINT nLength);
	void ByteToDWord(DWORD* Output, BYTE* Input, UINT nLength);

private:
	BYTE  m_lpszBuffer[64];  //input buffer
	ULONG m_nCount[2];   //number of bits, modulo 2^64 (lsb first)
	ULONG m_lMD5[4];   //MD5 checksum
};


/************************************************************************/
/* Use Case:
    char* p = "abcd";

	string md5Str = CMD5::MD5(p, strlen(p));
*/
/************************************************************************/
