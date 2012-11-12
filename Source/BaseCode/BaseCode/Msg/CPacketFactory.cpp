#include "CPacketFactory.h"
#include "CLog2.0.h"

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
	INITASSERT(NULL==pPacketObject)
		return;

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
	IFn(NULL==pPackHead)
		return NULL;

	return GetPacketObject(pPackHead->GetPacketDefine1());
}

bool CPacketFactory::ProcessMsg(IPackHead *pPackHead)
{
	IFn(NULL==pPackHead)
		return true;//数据出错，不代表网络出错

	IPacketObject* pPacketObject = g_PacketFactory.GetPacketObject(pPackHead);
	IFn(NULL==pPackHead)
	{
		LOGNE("CPacketFactory::ProcessMsg,NULL==pPackHead. PACKET_DEFINE1:%d,  \
			PACKET_DEFINE2:%d\n", pPackHead->GetPacketDefine1(), pPackHead->GetPacketDefine2());
		return true;//协议没有？
	}
	
	return pPacketObject->Execute(pPackHead);
}