/*  ���ߣ�		�ֶ�ƽ                                                              
	����ʱ�䣺	2010/07/14                                                         
    ����˵����  ����Э���壬����Э�鴦�������
*/

#pragma once
#include "PacketDefine.h"
#include "MTASSERT.h"
#include "CLog2.0.h"

/*
	IPackHeadΪЭ��Ļ��ࡣֱ����char m_Buffer[SOCKET_BUFF_SIZE];����ʾ���յ�����
	�ṹ����ʡ�����л��ͷ����л���
	�ϲ�Э��̳�IPackHead���ڹ��캯������ָ��ָ��m_pBuffer�����AddPacketSize��ʼʹ��
*/

/*
    ��λ:byte
	PACKET_BUFF_SIZE:���ð���ͨMTU=1500������
	PACKET_BUFF_SIZE_INNER:�߼���������֮��ͨѶ����ĳ��ȡ�����nNetKey��λ�õȡ�
	
	�ڶ���ʹ���жϵ�ʱ����PACKET_BUFF_SIZEΪ׼
*/
const int NET_PACKET_BUFF_SIZE = 1500*2; 
const int NET_PACKET_BUFF_SIZE_INNER = 20;

struct NET_PUBLIC_HEAD
{
	unsigned short m_nPacketSize;
	unsigned short m_nPacketDefine1;
	unsigned short m_nPacketDefine2;	
};

//ͷ����С����unsigned short m_nPacketSize
const int NET_PACKET_HEAD_SIZE = sizeof(NET_PUBLIC_HEAD);

class IPackHead
{
public:
	IPackHead();

	//��������Ѿ������½�һ��������Ҫ�����´˺���
	bool InitByCreate(const char* pDesBuffer, unsigned short nBufferSize);

	//����Ĵ���������ʾ�յ�����Ĵ���
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
	Э��Ĵ���������
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

