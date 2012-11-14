/*  作者：		林东平                                                              
	创建时间：	2010/07/14                                                         
    功能说明：  网络协议体，网络协议处理对象定义
*/

#pragma once
#include "PacketDefine.h"
#include "MTASSERT.h"
#include "CLog2.0.h"

/*
	IPackHead为协议的基类。直接用char m_Buffer[SOCKET_BUFF_SIZE];来表示最终的数据
	结构，节省了序列化和反序列化。
	上层协议继承IPackHead后，在构造函数中用指针指向m_pBuffer，配合AddPacketSize开始使用
*/


//按普通MTU=1500来配置
const int PACKET_BUFF_SIZE = 1500*2; 

class IPackHead
{
public:
	IPackHead();

	bool SetPacketDefine1(PACKET_DEFINE1 nPacketDefine1);
	bool SetPacketDefine2(PACKET_DEFINE2 nPacketDefine2);
	
	unsigned short GetPacketDefine1();
	unsigned short GetPacketDefine2();

	bool AddPacketSize(int nPacketSize);

	unsigned short GetPacketSize();
	char* GetPacketBuffer();


	//底层使用
	const char* K_GetPacketBuffer();
private:

public:

private:
	unsigned short m_nPacketSize;  //包大小，head+content
	unsigned short m_nPacketDefine1;
	unsigned short m_nPacketDefine2;	

	char m_Buffer[PACKET_BUFF_SIZE];
	char* m_pBuffer;
};



/*
	协议的处理对象基类
*/
class IPacketObject
{
public:
	PACKET_DEFINE1 m_nPacketDefine;	

private:

public:
	IPacketObject(PACKET_DEFINE1 nPacketDefine);
	PACKET_DEFINE1 GetPacketID();

	/*
		RoleObject在处理Accept事件时表示RoleNet对象,
		否则表示Role对象
	*/
	virtual int	Execute(IPackHead* pPackHead)=0;

private:


};

