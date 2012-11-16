/*  ���ߣ�		�ֶ�ƽ                                                              
	����ʱ�䣺	2010/07/14   5                                                     
    ����˵����  ���������������Ҫ��Ϊ�����η��ӵ����кţ����ض�Ӧ��Э�鴦�����
*/

#pragma once


#include "Singleton.h"
#include "PPackets.h"


class CPacketFactory:public Singleton<CPacketFactory>
{
public:
	CPacketFactory();
	~CPacketFactory();

	void AddPacketObject(IPacketObject* pPacketObject);
	void ProcessMsg(IPackHead *pPackHead);

	IPacketObject* GetPacketObject( int e );
	

private:
	IPacketObject* GetPacketObject( IPackHead *pPackHead );

public:

private:
	bool m_bFactory[PACKET1_MAX];
	IPacketObject* m_Factory[PACKET1_MAX];
	int m_nFactorySize;

};

#define g_PacketFactory CPacketFactory::getSingleton()