/*  作者：		lin_style lin_style@foxmail.com                                                              
	创建时间：	2012/11/14                                                         
    功能说明：  逻辑层和网络层的通讯协议
*/

#pragma  once
#include "PPackets.h"
#include "CNetDriver2.h"
#include "CPacketFactory.h"
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

struct P_INNER_TRANSFER_ERR:public NET_PUBLIC_HEAD
{
	/*
		m_nNetKey:网络key
	*/
	unsigned int m_nNetKey;
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
class PInnerTransfer:public IPacketHead
{
public:
	PInnerTransfer();

	unsigned int GetNetKey();
	P_INNER_TRANSFER* GetInnerTransferPacket();

	//网络到逻辑层，这里提升效率，不用外部考别，直接GetPacketBuffer()出去考别
	bool CreateNtoL(unsigned int nNetKey);
	bool CreateNtoLErr(unsigned int nNetKey);

	//逻辑到网络层
	bool CreateLtoN(unsigned int nNetKey, const char* pLogicPacket, unsigned short nLogicPacketSize);
	bool CreateLtoNErr(unsigned int nNetKey);
	
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
class POInnerTransfer:public Singleton<POInnerTransfer>, public IPacketObject
{ 
public:
	POInnerTransfer():IPacketObject(PACKET1_INNER_NET_LOGIC_QUEUE)
	{
		g_PacketFactory.AddPacketObject(this);
	}

	virtual void Execute(IPacketHead* pPackHead) ;
};