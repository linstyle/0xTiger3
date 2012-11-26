#include "CPacketFactory.h"
#include "GlobalFunction.h"
#include "CLog2.0.h"


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

IPacketObject* CPacketFactory::GetPacketObject( IPacketHead *pPacketHead )
{
	IFn(NULL==pPacketHead)
		return NULL;

	return GetPacketObject(pPacketHead->GetPacketDefine1());
}

void CPacketFactory::ProcessMsg(IPacketHead *pPacketHead)
{
	IFn(NULL==pPacketHead)
		return;

	IPacketObject* pPacketObject = g_PacketFactory.GetPacketObject(pPacketHead);	
	IFn(NULL==pPacketObject)
	{
		//协议没有？
		LOGNE("CPacketFactory::ProcessMsg,NULL==pPacketObject. PACKET_DEFINE1:%d,  \
			PACKET_DEFINE2:%d\n", pPacketHead->GetPacketDefine1(), pPacketHead->GetPacketDefine2());
		return;
	}

	IFn(pPacketHead->GetPacketDefine1()!=pPacketObject->GetPacketDefine1())
	{
		//协议不一致？
		//协议没有？
		LOGNE("CPacketFactory::ProcessMsg,pPacketHead->Define1!=pPacketObject->Define1. PACKET_DEFINE1:%d,  \
			  PACKET_DEFINE2:%d\n", pPacketHead->GetPacketDefine1(), pPacketHead->GetPacketDefine2());
		return;
	}

	
	pPacketObject->Execute(pPacketHead);
}