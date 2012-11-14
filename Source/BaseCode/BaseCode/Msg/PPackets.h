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


//����ͨMTU=1500������
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


	//�ײ�ʹ��
	const char* K_GetPacketBuffer();
private:

public:

private:
	unsigned short m_nPacketSize;  //����С��head+content
	unsigned short m_nPacketDefine1;
	unsigned short m_nPacketDefine2;	

	char m_Buffer[PACKET_BUFF_SIZE];
	char* m_pBuffer;
};



/*
	Э��Ĵ���������
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
		RoleObject�ڴ���Accept�¼�ʱ��ʾRoleNet����,
		�����ʾRole����
	*/
	virtual int	Execute(IPackHead* pPackHead)=0;

private:


};

