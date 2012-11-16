/*  作者：		lin_style lin_style@foxmail.com                                                              
	创建时间：	2012/11/14                                                         
    功能说明：  逻辑层和网络层的通讯协议
*/

#pragma  once
#include "PPackets.h"

/****************************
	你的结构
	1)以P_打头
	2)然后继承NET_PUBLIC_HEAD
*****************************/
struct P_INNER_TRANSFER:public NET_PUBLIC_HEAD
{
	/*
		m_nNetKey:网络key
		m_NetPacket:传输的网络包
	*/
	unsigned int m_nNetKey;
	char* m_NetPacket;
};

/****************************
	你的PACKET_DEFINE2定义,即协议大类下的小协议分支
	1)以D2打头
*****************************/
enum
{
	D2_INNER_N_TO_L_SEND = 0,  //网络层给逻辑层，需要发送出去
	D2_INNER_N_TO_L_ERR = 1,   //网络层给逻辑层的包，网络层出错

	D2_INNER_L_TO_N_SEND = 2,  //逻辑层给网络层,需要发送出去
	D2_INNER_L_TO_N_ERR = 3,   //逻辑层给网络层,需要发送出去
};

/****************************
	你的协议类定义,继承IPackHead
	1)以P打头
*****************************/
class PInnerTransfer:public IPackHead
{
public:
	PInnerTransfer();

	virtual void Process();

	//创建一个网络到逻辑层的包
	bool CreateNtoL(unsigned int nNetKey, const char* pNetPacket, unsigned short nNetPacketSize);

private:

public:

private:
	union
	{
		void* m_pUnionBuffer;
		P_INNER_TRANSFER* m_pInnerTransfer;
	};
	
};

/****************************
	你的包工厂定义,用于管理收到包后的处理
	继承IPacketObject和一个单体
*****************************/
class PInnerTransfer:public Singleton<PInnerTransfer>, public IPacketObject
{ 
public:
	PLoginGLObject():IPacketObject(PACKET1_INNER_NET_LOGIC_QUEUE)
	{
		g_PacketFactory.AddPacketObject(this);
	}

	virtual int	Execute(IPackHead* pPackHead) ;
};