#include "stdafx.h"
#include "CObjectMapManager.h"
#include "NetAPI.h"
#include "CWindowsSlabManager.h"

using namespace name_objectmap;
initialiseSingleton(CObjectMapManager);

CObjectMapManager::CObjectMapManager()
{
	
}

void CObjectMapManager::AddPlayer(INT64 nUserID, void *pNetObject, CBaseLogicPlayer* pBaseLogicPlayer)
{
	IF (VerifyUserValid(nUserID))
	{
		//	TRACER_STACK();
		return ;
	}

	OBJECT_VALUE* pObjectValue = (OBJECT_VALUE*)name_slab::kmem_malloc(sizeof(OBJECT_VALUE));
	pObjectValue->m_pBaseLogicPlayer = pBaseLogicPlayer;
	pObjectValue->m_pNetObject = pNetObject;

	m_mapUserIDObject.insert(pair<UINT,  OBJECT_VALUE*>(nUserID, pObjectValue));
}

void CObjectMapManager::SendByUser(INT64 nUserID, const char* pBuffer, int nBufferLen)
{
	IF(NULL==pBuffer)
	{
		TRACER_STACK();
		return;
	}

	OBJECT_VALUE *pObjectValue = GetObjectValueByUserID(nUserID);
	if (NULL==pObjectValue)
	{
		LOGE("ObjectMapManager::SendByUser:NULL==pObjectValue. UserID:%I64d\n", nUserID);
		return;
	}

	if (NULL==pObjectValue->m_pBaseLogicPlayer || NULL==pObjectValue->m_pNetObject)
	{
		LOGE("ObjectMapManager::SendByUser:NULL==pObjectValue->. UserID:%I64d\n", nUserID);
		return;
	}

	if( pObjectValue->m_pBaseLogicPlayer->GetUserID()!=nUserID )
	{
		LOGE("ObjectMapManager::SendByUser:ID Mismatching. UserID1:%I64d, UserID2:%I64d\n", 
			pObjectValue->m_pBaseLogicPlayer->GetUserID(), nUserID);
		return;
	}

	IF( -1==PutPacketStream(pBuffer, nBufferLen,  pObjectValue->m_pNetObject) )
	{
		
	}
}

void CObjectMapManager::CloseByUser(INT64 nUserID)
{
	OBJECT_VALUE *pObjectValue = GetObjectValueByUserID(nUserID);
	if (NULL==pObjectValue)
	{
		LOGE("ObjectMapManager::CloseByUser:NULL==pObjectValue. UserID:%I64d\n", nUserID);
		return;
	}

	name_slab::kmem_free( (void*)pObjectValue);

	m_mapUserIDObject.erase(nUserID);
}

bool CObjectMapManager::VerifyUserValid(INT64 nUserID)
{
	if (m_mapUserIDObject.end()==m_mapUserIDObject.find(nUserID))
	{
		return false;
	}

	return true;
}

bool CObjectMapManager::VerifySendPacket(const IPackHead *pPackHead, int nBufferLen)
{
	if( pPackHead->GetPacketSize()>name_msg_packet::SOCKET_BUFF_SIZE )
	{
		LOGE("Err, CNetObjectManager::VerifySendPacket. PacketID1:%d, PacketID2:%d, PacketSize:%d>%d", 
			pPackHead->GetPacketDefine1(), pPackHead->GetPacketDefine2(),
			pPackHead->GetPacketSize(), name_msg_packet::SOCKET_BUFF_SIZE);

		TRACER_STACK();
		return false;		
	}

	IF( pPackHead->GetPacketSize()!=nBufferLen )
	{
		LOGE("Err, CNetObjectManager::VerifySendPacket. PacketID1:%d, PacketID2:%d, PacketSize:%d!=%d", 
			pPackHead->GetPacketDefine1(), pPackHead->GetPacketDefine2(),
			pPackHead->GetPacketSize(), nBufferLen);
		TRACER_STACK();
		return false;				
	}

	return true;
}

int CObjectMapManager::PutPacketStream(const char *pBuffer, int nBufferLen, void *pNetObject)
{
	IPackHead *pPackHead = (IPackHead*) pBuffer;

	if (false==VerifySendPacket(pPackHead, nBufferLen))
	{
		return -1;
	}

	pPackHead->SetNetObject(pNetObject);

	return net::PutPacketStream(pBuffer, nBufferLen);
}

OBJECT_VALUE* CObjectMapManager::GetObjectValueByUserID(INT64 nUserID)
{
	IF (VerifyUserValid(nUserID))
	{
		//	TRACER_STACK();
		return NULL;
	}

	MAP_USERID_OBJECT::iterator ite = m_mapUserIDObject.find(nUserID);
	return ite->second;
}

bool CObjectMapManager::ChangeIdent(INT64 nUserID, IDENT nIdent)
{
	
	return false;
}

void CObjectMapManager::SendToLS(const char *pBuffer, int nBufferLen)
{
	//IF(-1==PutPacketStream(pBuffer, nBufferLen, m_pLSNetObject))
	//{
	//	return;
	//}

}


//void CNetObjectManager::TracerMe()
//{
//	LOGN("----------Notic,    CNetObjectManager::TracerMe()\n")
//	LOGN("");
//	LOGN("\n----------Notic,   end CNetObjectManager::TracerMe()\n")
//}