/*  作者：		lin_style lin_style@foxmail.com                                                              
	创建时间：	2012/11/14                                                         
    功能说明：  逻辑层和网络层的通讯协议
*/

#pragma  once
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
//该协议包给网络层
class PInnerTransferToNet:public IPacketHead
{
public:
	PInnerTransferToNet();

	unsigned int GetNetKey();
	P_INNER_TRANSFER* GetInnerTransferPacket();
	P_INNER_TRANSFER_ERR* GetErrInnerTransferPacket();

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
		P_INNER_TRANSFER_ERR* m_pErrInnerTransfer;
	};
	
};

/****************************
	你的包工厂定义,用于管理收到包后的处理
	继承IPacketObject和一个单体
	*****************************/
class POInnerTransferToNet:public Singleton<POInnerTransferToNet>, public IPacketObject
{ 
public:
	POInnerTransferToNet():IPacketObject(PACKET1_INNER_TO_NET)
	{
		g_NetPacketFactory.AddPacketObject(this);
	}

	virtual void Execute(IPacketHead* pPacketHead) ;
};



