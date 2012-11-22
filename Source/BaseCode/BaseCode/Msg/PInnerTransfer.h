/*  ���ߣ�		lin_style lin_style@foxmail.com                                                              
	����ʱ�䣺	2012/11/14                                                         
    ����˵����  �߼����������ͨѶЭ��
*/

#pragma  once
#include "PPackets.h"
#include "CNetDriver2.h"
#include "CPacketFactory.h"
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

struct P_INNER_TRANSFER_ERR:public NET_PUBLIC_HEAD
{
	/*
		m_nNetKey:����key
	*/
	unsigned int m_nNetKey;
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
class PInnerTransfer:public IPacketHead
{
public:
	PInnerTransfer();

	unsigned int GetNetKey();
	P_INNER_TRANSFER* GetInnerTransferPacket();

	//���絽�߼��㣬��������Ч�ʣ������ⲿ����ֱ��GetPacketBuffer()��ȥ����
	bool CreateNtoL(unsigned int nNetKey);
	bool CreateNtoLErr(unsigned int nNetKey);

	//�߼��������
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
	��İ���������,���ڹ����յ�����Ĵ���
	�̳�IPacketObject��һ������
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