/*  ���ߣ�		lin_style lin_style@foxmail.com                                                              
	����ʱ�䣺	2012/11/14                                                         
    ����˵����  �߼����������ͨѶЭ��
*/

#pragma  once
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
class PInnerTransferToLogic:public IPacketHead
{
public:
	PInnerTransferToLogic();

	unsigned int GetNetKey();
	P_INNER_TRANSFER* GetInnerTransferPacket();
	P_INNER_TRANSFER_ERR* GetErrInnerTransferPacket();

	//���絽�߼��㣬��������Ч�ʣ������ⲿ������ֱ��GetPacketBuffer()��ȥ����
	bool CreateNtoL(unsigned int nNetKey);
	bool CreateNtoLErr(unsigned int nNetKey);
	
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
	��İ���������,���ڹ����յ�����Ĵ���
	�̳�IPacketObject��һ������
	*****************************/
class POInnerTransferToLogic:public Singleton<POInnerTransferToLogic>, public IPacketObject
{ 
public:
	POInnerTransferToLogic():IPacketObject(PACKET1_INNER_TO_LOGIC)
	{
		g_LoigcPacketFactory.AddPacketObject(this);
	}

	virtual void Execute(IPacketHead* pPacketHead) ;
};
