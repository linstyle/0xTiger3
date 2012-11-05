#include "StdAfx.h"
#include "CNetTransToLogic.h"

initialiseSingleton(CNetTransToLogic);

CNetTransToLogic::CNetTransToLogic()
{
	m_NetLogicMap.Reserve(8000);
}

int CNetTransToLogic::AcceptConnect(CIOCPData* pIOCPKey)
{
	void *pRole;

	IPacketObject* pPacketObject = g_PacketFactory.GetPacketObject(PACKET1_ACCEPT);
	if( NULL==pPacketObject )
	{
		printf("Err,CNetTransToLogic::AcceptConnect:GetPacketObject\n");
		return -1;
	}

	pRole =(void*) pPacketObject->Execute((IPackHead*)pIOCPKey->BufferPacket, (void*)pIOCPKey);
	if(NULL==pRole)
	{
		printf("Err,CNetTransToLogic::AcceptConnect:Execute, NULL==Malloc\n");
		return -1;
	}

	/*
		m_NetLogicMap内部空间可能不足
	*/
	m_Lock.Lock();
	if(false==m_NetLogicMap.Insert(pIOCPKey, pRole))
	{
		printf("Warning, CNetTransToLogic::AcceptConnect:Insert==NULL\n");
		m_Lock.UnLock();
		return -1;
	}
	m_Lock.UnLock();

	return 0;
}

int CNetTransToLogic::ExceptionPacket(CIOCPData* pIOCPKey)
{
	/*
		取到逻辑对象
	*/
	void *pRole = m_NetLogicMap.Find(pIOCPKey);
	if( NULL==pRole )
	{
		/*
			有可能刚连接就报错，所以找不到逻辑对象
		*/
		printf("Err, CNetTransToLogic::ExceptionPacket:Find==NULL\n");
		return -1;
	}
	/*
		从NetLogicMap中删除
	*/
	m_Lock.Lock();
	if( -1==m_NetLogicMap.Erase(pIOCPKey) )
	{
		printf("Err, CNetTransToLogic::ExceptionPacket:Erase==NULL\n");
		m_Lock.UnLock();
		return -1;	
	}
	m_Lock.UnLock();

	/*
		取到该协议的处理对象
	*/
	IPacketObject* pPacketObject = g_PacketFactory.GetPacketObject(PACKET1_EXCEPTION);
	if( NULL==pPacketObject )
	{
		printf("Err, CNetTransToLogic::ExceptionPacket:GetPacketObject\n");
		return -1;
	}

	return pPacketObject->Execute((IPackHead*)pIOCPKey->BufferPacket, pRole);		
}

int CNetTransToLogic::ExcutePacket(CIOCPData* pIOCPKey)
{
	/*
		取到逻辑对象
	*/
	void *pRole = m_NetLogicMap.Find(pIOCPKey);
	if( NULL==pRole )
	{
		printf("Err, CNetTransToLogic::ExcutePacket:Find==NULL\n");
		return -1;
	}

	/*
		取到该协议的处理对象
	*/
	IPackHead* pPackBase = (IPackHead*)pIOCPKey->BufferPacket;
	IPacketObject* pPacketObject = g_PacketFactory.GetPacketObject( pPackBase->GetPacketDefine1());
	if( NULL==pPacketObject )
	{
		pPacketObject = g_PacketFactory.GetPacketObject( PACKET1_EXCEPTION);
		pPacketObject->Execute(pPackBase, pRole);

		printf("Err, CNetTransToLogic::ExcutePacket:GetPacketObject(id:%d)==NULL\n", pPackBase->GetPacketDefine1());

		return -1;
	}

	return pPacketObject->Execute(pPackBase, pRole);	
}

