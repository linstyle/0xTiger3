/*  ���ߣ�		�ֶ�ƽ                                                              
	����ʱ�䣺	2010/07/14   5                                                     
    ����˵����  ���������������Ҫ��Ϊ�����η��ӵ����кţ����ض�Ӧ��Э�鴦�����
*/

#pragma once

#include "PPackets.h"


class CPacketFactory
{
public:
	CPacketFactory();
	~CPacketFactory();

	void AddPacketObject(IPacketObject* pPacketObject);
	/*
		ProcessMsg���ﷵ��ֵ������Ϊbool.
		��Ϊ�ڲ����߼������ʾֵ�кܶ࣬����������ϣ�һЩ��������ȡ����ǲ�����ʾ
		��Ҫ����falseȥ�Ͽ�������ӣ�������������̫ϸ�¡�����Ƿ�Ͽ������ڲ��߼�
		�Լ�����
	*/
	void ProcessMsg(IPacketHead *pPacketHead);

	IPacketObject* GetPacketObject( int e );
	

private:
	IPacketObject* GetPacketObject( IPacketHead *pPacketHead );

public:

private:
	bool m_bFactory[PACKET1_MAX];
	IPacketObject* m_Factory[PACKET1_MAX];
	int m_nFactorySize;

};

extern CPacketFactory* g_LoigcPacketFactory;
extern CPacketFactory* g_NetPacketFactory;