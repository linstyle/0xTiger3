#include "CPacketFactory.h"
#include "CLog2.0.h"

//initialiseSingleton(CPacketFactory);
//
//CPacketFactory::CPacketFactory()
//{
//	m_nFactorySize = PACKET1_MAX;
//	MEMSET(m_bFactory, 0, PACKET1_MAX);
//}
//CPacketFactory::~CPacketFactory()
//{
//	MEMSET(m_Factory, 0, sizeof(m_Factory));
//}
//
//void CPacketFactory::AddPacketObject(IPacketObject* pPacketObject)
//{
//	INITASSERT(NULL==pPacketObject);
//
//	m_Factory[pPacketObject->GetPacketID()] = pPacketObject;
//	m_bFactory[pPacketObject->GetPacketID()] = true;
//}
//
//IPacketObject* CPacketFactory::GetPacketObject( int e )
//{	
//	if( (e<=PACKET1_MIN || e>=PACKET1_MAX) || false==m_bFactory[e] )
//	{
//		return NULL;
//	}
//	
//	return m_Factory[e];
//}
//
//IPacketObject* CPacketFactory::GetPacketObject( IPackHead *pPackHead )
//{
//	IFn(NULL==pPackHead)
//		return NULL;
//
//	return GetPacketObject(pPackHead->GetPacketDefine1());
//}
//
//void CPacketFactory::ProcessMsg(IPackHead *pPackHead)
//{
//	IFn(NULL==pPackHead)
//		return;
//
//	IPacketObject* pPacketObject = g_PacketFactory.GetPacketObject(pPackHead);
//	IFn(NULL==pPackHead)
//	{
//		LOGNE("CPacketFactory::ProcessMsg,NULL==pPackHead. PACKET_DEFINE1:%d,  \
//			PACKET_DEFINE2:%d\n", pPackHead->GetPacketDefine1(), pPackHead->GetPacketDefine2());
//		return;//协议没有？
//	}
//	
//	pPacketObject->Execute(pPackHead);
//}