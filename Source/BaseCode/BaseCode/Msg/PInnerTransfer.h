/*  ���ߣ�		lin_style lin_style@foxmail.com                                                              
	����ʱ�䣺	2012/11/14                                                         
    ����˵����  �߼����������ͨѶЭ��
*/

#pragma  once
#include "PPackets.h"

/****************************
	��Ľṹ
	1)��P_��ͷ
	2)Ȼ��̳�NET_PUBLIC_HEAD
*****************************/
struct P_INNER_TRANSFER:public NET_PUBLIC_HEAD
{
	/*
		m_nNetKey:����key
		m_NetPacket:����������
	*/
	unsigned int m_nNetKey;
	char* m_NetPacket;
};

/****************************
	���PACKET_DEFINE2����,��Э������µ�СЭ���֧
	1)��D2��ͷ
*****************************/
enum
{
	D2_INNER_N_TO_L_SEND = 0,  //�������߼��㣬��Ҫ���ͳ�ȥ
	D2_INNER_N_TO_L_ERR = 1,   //�������߼���İ�����������

	D2_INNER_L_TO_N_SEND = 2,  //�߼���������,��Ҫ���ͳ�ȥ
	D2_INNER_L_TO_N_ERR = 3,   //�߼���������,��Ҫ���ͳ�ȥ
};

/****************************
	���Э���ඨ��,�̳�IPackHead
	1)��P��ͷ
*****************************/
class PInnerTransfer:public IPackHead
{
public:
	PInnerTransfer();

	virtual void Process();

	//����һ�����絽�߼���İ�
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
	��İ���������,���ڹ����յ�����Ĵ���
	�̳�IPacketObject��һ������
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