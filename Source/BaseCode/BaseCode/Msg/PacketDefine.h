/*  ���ߣ�		lin_style lin_style@foxmail.com                                                              
	����ʱ�䣺	2012/11/14                                                         
    ����˵����  Э�鶨��ö�ٹ����ļ�
*/
#pragma once

/*
	@PACKET_DEFINE

	Client  C   �ͻ���
	GatewayServer W
	GameServer G
	DataServer  D
	I  �߼����������ڲ�ͨѶ

	������������£�Ԥ�������࣬�������Ӳ���Ҫ��Э���ļ���:
	PACKET_DEFINE2��ΪPACKET_DEFINE1������.ע����������
*/
enum PACKET_DEFINE1
{
	PACKET1_MIN,
	PACKET1_INNER_NET_LOGIC = 0,   //�ڲ�ͨѶ���������߼���

	PACKET1_MAX = 65536,
};
