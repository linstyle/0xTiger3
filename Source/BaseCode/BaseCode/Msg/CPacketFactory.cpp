#include "CPacketFactory.h"


initialiseSingleton(CPacketFactory);

CPacketFactory::CPacketFactory()
{
	m_nFactorySize = PACKET1_MAX;
	memset(m_bFactory, 0, PACKET1_MAX);
}
CPacketFactory::~CPacketFactory()
{
	memset(m_Factory, 0, sizeof(m_Factory));
}

void CPacketFactory::AddPacketObject(IPacketObject* pPacketObject)
{
	m_Factory[pPacketObject->GetPacketID()] = pPacketObject;
	m_bFactory[pPacketObject->GetPacketID()] = true;
}

IPacketObject* CPacketFactory::GetPacketObject( int e )
{	
	if( (e<=PACKET1_MIN || e>=PACKET1_MAX) || false==m_bFactory[e] )
	{
		return NULL;
	}
	
	return m_Factory[e];
}

IPacketObject* CPacketFactory::GetPacketObject( IPackHead *pPackHead )
{
	return GetPacketObject(pPackHead->GetPacketDefine1());
}