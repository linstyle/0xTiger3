#include "StdAfx.h"
#include "PNLInnerNotic.h"
#include "CObjectMapManager.h"

initialiseSingleton(PNLInnerNoticObject);

int	PNLInnerNoticObject::Execute(IPackHead* pPackHead)
{
	switch (pPackHead->GetPacketDefine2())
	{
	case PACKET2_NTOL_ACCEPT_SOCKET:  //有新的连接
		LOGN("PNLInnerNoticObject::Execute.PACKET2_NTOL_ACCEPT_SOCKET :%x\n", pPackHead);

		break;
	case  PACKET2_NTOL_ERR: //网络层投递的出错包
		LOGN("PNLInnerNoticObject::Execute.PACKET2_NTOL_ERR  :%x\n", pPackHead);
		break;

	default:
		//IF(pPackHead->GetPacketDefine2())
		LOGE("PNLInnerNoticObject::Execute. case default:%d\n", pPackHead->GetPacketDefine2());

	}
	return 0;
}
