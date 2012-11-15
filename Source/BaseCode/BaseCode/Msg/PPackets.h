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

/*
    单位:byte
	PACKET_BUFF_SIZE:设置按普通MTU=1500来配置
	PACKET_BUFF_SIZE_INNER:逻辑层和网络层之间通讯额外的长度。比如nNetKey的位置等。
	
	在对外使用判断的时候以PACKET_BUFF_SIZE为准
*/
const int NET_PACKET_BUFF_SIZE = 1500*2; 
const int NET_PACKET_BUFF_SIZE_INNER = 20;

struct NET_PUBLIC_HEAD
{
	unsigned short m_nPacketSize;
	unsigned short m_nPacketDefine1;
	unsigned short m_nPacketDefine2;	
};

//头部大小，即unsigned short m_nPacketSize
const int NET_PACKET_HEAD_SIZE = sizeof(NET_PUBLIC_HEAD);

class IPackHead
{
public:
	IPackHead();

	//如果是用已经数据新建一个包，需要调用下此函数
	bool InitByCreate(const char* pDesBuffer, unsigned short nBufferSize);

	//纯虚的处理函数，表示收到包后的处理
	virtual void Process() = 0;
	
	char* GetPacketBuffer();
	unsigned short GetPacketSize(); 
	unsigned short GetPacketDefine1();
	unsigned short GetPacketDefine2();
private:

public:

private:
	union
	{
		char m_Buffer[NET_PACKET_BUFF_SIZE+NET_PACKET_BUFF_SIZE_INNER];
		NET_PUBLIC_HEAD m_PacketHead;
	};
	
};



/*
	协议的处理对象基类
*/
//class IPacketObject
//{
//public:
//	PACKET_DEFINE1 m_nPacketDefine;	
//
//private:
//
//public:
//	IPacketObject(PACKET_DEFINE1 nPacketDefine);
//	PACKET_DEFINE1 GetPacketID();
//
//	virtual int	Execute(IPackHead* pPackHead)=0;
//
//private:
//
//
//};

