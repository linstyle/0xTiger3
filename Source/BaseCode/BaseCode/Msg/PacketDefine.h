/*  作者：		lin_style lin_style@foxmail.com                                                              
	创建时间：	2012/11/14                                                         
    功能说明：  协议定义枚举公共文件
*/
#pragma once

/*
	@PACKET_DEFINE

	Client  C   客户端
	GatewayServer W
	GameServer G
	DataServer  D
	I  逻辑层和网络层内部通讯

	包定义设计如下，预定好两类，以免增加不必要的协议文件类:
	PACKET_DEFINE2作为PACKET_DEFINE1的子类.注意命名规则
*/
enum PACKET_DEFINE1
{
	PACKET1_MIN,
	PACKET1_INNER_TO_NET = 0,			 //内部通讯，逻辑层给网络层
	PACKET1_INNER_TO_LOGIC = 1,			 //内部通讯，网络层给逻辑层	

	PACKET1_MAX = 1000,  //预分配不要太大，跟CPacketFactory数组挂钩
};
