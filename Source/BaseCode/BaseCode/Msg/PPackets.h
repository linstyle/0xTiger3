/*  作者：		林东平                                                              
	创建时间：	2010/07/14                                                         
    功能说明：  网络协议体，网络协议处理对象定义
*/

#pragma once
#include "MTASSERT.h"

/*
	@PACKET_DEFINE

	Client  C   客户端
	LoginServer L
	WeightServer W
	GatewayServer G
	PhonebookServer P
	GameServer G
	DataServer  D
	ChatServer  C

	包定义设计如下，预定好两类，以免增加不必要的协议文件类:
	PACKET_DEFINE2作为PACKET_DEFINE1的子类.注意命名规则
*/
enum PACKET_DEFINE1
{
	PACKET1_MIN,
	/* 0-4 */ //内部网络层和逻辑层的通讯
	PACKET1_INNER_NET_LOGIC,        //内部通讯，网络层和逻辑层
	PACKET1_LOGIN,                           //登陆协议
	
	PACKET1_HELLO,				               //每个服务器组件固定的协议实现,刚连接时发送一句问候
	PACKET1_REHELLO,			               //返回该问候
	PACKET1_FILE_TRANS,		          //测试文件发送
	PACKET1_ACCEPT,				          //新对象连接进来
	PACKET1_EXCEPTION,		              //网络出错，需要关闭的对象

	PACKET1_LOGIN_GL,			         //GS登陆到LS
	PACKET1_LOGIN_CG,			         //ChatServer登陆到GateWay

	PACKET1_MAX
};

enum PACKET_DEFINE2
{
	PACKET2_MIN = 0,
	//-----------PACKET1_HELLO
	PACKET2_HELLO_GL_SAY,
	PACKET2_HELLO_GL_RESAY,

	//-----------PACKET1_INNER_NET_LOGIC
	PACKET2_NTOL_ACCEPT_SOCKET,           //网络层接收到一个链接
	PACKET2_NTOL_ERR,                                //网络层投递的出错包
	PACKET2_LTON_SERVER_SOCKET,           //逻辑层投递的server套接字包
	PACKET2_LTON_CONNECT_SOCKET,       //逻辑层投递的client套接字包
	PACKET2_LTON_ERR,                                //逻辑层投递的出错包
	PACKET2_MAX,

	//-----------PACKET1_LOGIN
	PACKET2_LOGIN_GL,                        //GS向LS登陆

};

/*
	协议的头部
	该类的大小，放于后期优化
*/
namespace name_msg_packet
{
	/*
		 byte，每个网络包最大字节数,具体跟MTU相关。以普通的MTU 1400为例，定为2800
	*/
	const unsigned int SOCKET_BUFF_SIZE = 2800; 
	const unsigned int PACKET_HEAD_SIZE = sizeof(IPackHead);
};

class IPackHead
{
public:
	IPackHead(PACKET_DEFINE1 nPacketDefine1, int nPacketSize);

	void SetNetKey(unsigned int nNetKey);
	unsigned int  GetNetKey();

	void SetPacketDefine2(PACKET_DEFINE2 nPacketDefine2);
	void SetPacketSize(int nPacketSize);

	PACKET_DEFINE1 GetPacketDefine1();
	PACKET_DEFINE2 GetPacketDefine2();

	//包大小，head+content
	int GetPacketSize();
	int GetPacketContentSize();

private:

public:

private:
	int m_nPacketSize;  //包大小，head+content
	PACKET_DEFINE1 m_nPacketDefine1;
	PACKET_DEFINE2 m_nPacketDefine2;
	int m_nRoutePriority;//路由转发的等级, 需要手动设置
	
	unsigned int  m_nNetKey; //网络层的KEY

	char m_Buffer[SOCKET_BUFF_SIZE];
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

