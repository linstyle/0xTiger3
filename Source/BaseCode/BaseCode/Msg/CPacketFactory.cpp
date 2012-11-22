#include "CPacketFactory.h"
#include "CLog2.0.h"
#include "mystdio.h"

initialiseSingleton(CPacketFactory);

CPacketFactory::CPacketFactory()
{
	m_nFactorySize = PACKET1_MAX;
	MEMSET(&m_bFactory, 0, sizeof(m_bFactory));
}
CPacketFactory::~CPacketFactory()
{
	MEMSET(&m_Factory, 0, sizeof(m_Factory));
}

void CPacketFactory::AddPacketObject(IPacketObject* pPacketObject)
{
	INITASSERT(NULL==pPacketObject);

	m_Factory[pPacketObject->GetPacketDefine1()] = pPacketObject;
	m_bFactory[pPacketObject->GetPacketDefine1()] = true;
}

IPacketObject* CPacketFactory::GetPacketObject( int e )
{	
	if( (e<=PACKET1_MIN || e>=PACKET1_MAX) || false==m_bFactory[e] )
	{
		return NULL;
	}
	
	return m_Factory[e];
}

IPacketObject* CPacketFactory::GetPacketObject( IPacketHead *pPackHead )
{
	IFn(NULL==pPackHead)
		return NULL;

	return GetPacketObject(pPackHead->GetPacketDefine1());
}

void CPacketFactory::ProcessMsg(IPacketHead *pPackHead)
{
	IFn(NULL==pPackHead)
		return;

	IPacketObject* pPacketObject = g_PacketFactory.GetPacketObject(pPackHead);	
	IFn(NULL==pPacketObject)
	{
		//协议没有？
		LOGNE("CPacketFactory::ProcessMsg,NULL==pPacketObject. PACKET_DEFINE1:%d,  \
			PACKET_DEFINE2:%d\n", pPackHead->GetPacketDefine1(), pPackHead->GetPacketDefine2());
		return;
	}

	IFn(pPackHead->GetPacketDefine1()!=pPacketObject->GetPacketDefine1())
	{
		//协议不一致？
		//协议没有？
		LOGNE("CPacketFactory::ProcessMsg,pPackHead->Define1!=pPacketObject->Define1. PACKET_DEFINE1:%d,  \
			  PACKET_DEFINE2:%d\n", pPackHead->GetPacketDefine1(), pPackHead->GetPacketDefine2());
		return;
	}

	
	pPacketObject->Execute(pPackHead);
}