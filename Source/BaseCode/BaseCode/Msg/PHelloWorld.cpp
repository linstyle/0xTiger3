#include "stdafx.h"
#include "PHelloWorld.h"
#include "CObjectMapManager.h"

initialiseSingleton(PHelloWorldObject);

int	PHelloWorldObject::Execute(IPackHead* pPackHead)
{
	IF(pPackHead->GetPacketDefine1()!=PACKET1_HELLO)
	{
		return -1;
	}

	PHelloWorld *pRecvMsg = (PHelloWorld*)pPackHead;
	PHelloWorld msgHelloWorld;

	switch (pPackHead->GetPacketDefine2())
	{
	case PACKET2_HELLO_GL_SAY:
		LOGN("PHelloWorldObject::Execute:PACKET2_HELLO_GL_SAY, %s\n",  pRecvMsg->m_Buffer);

		msgHelloWorld.SetPacketDefine2(PACKET2_HELLO_GL_RESAY);
		strncpy(msgHelloWorld.m_Buffer, "PACKET2_HELLO_GL_RESAY", sizeof(msgHelloWorld.m_Buffer));

		g_NetObjectManager.SendToLS((char*)&msgHelloWorld, msgHelloWorld.GetPacketSize());
		break;

	case PACKET2_HELLO_GL_RESAY:
		LOGN("PHelloWorldObject::Execute:PACKET2_HELLO_GL_RESAY, %s\n",  pRecvMsg->m_Buffer);
		break;

	default:
		LOGE("PHelloWorldObject::Execute. case default:%d. add:%x\n", pPackHead->GetPacketDefine2(), pPackHead);

	}
	return 0;
}