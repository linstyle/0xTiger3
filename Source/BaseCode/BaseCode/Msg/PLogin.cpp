#include "stdafx.h"
#include "PLogin.h"
#include "CObjectMapManager.h"

initialiseSingleton(PLoginGLObject);


int	PLoginGLObject::Execute(IPackHead* pPackHead)
{
	switch (pPackHead->GetPacketDefine2())
	{
	case PACKET2_LOGIN_GL:
	//	g_NetObjectManager.AddLS(pPackHead->GetNetObject());
		LOGN("PLoginGLObject::Execute, PACKET2_LOGIN_GL.add:%x \n", pPackHead);

		break;
	default:
		//IF(pPackHead->GetPacketDefine2())
		LOGE("PLoginGLObject::Execute. case default:%d. add:%x\n", pPackHead->GetPacketDefine2(), pPackHead);

	}
	return 0;
}