#include "CLogicDriver.h"
#include "NetAPI.h"
#include "CPacketFactory.h"
#include "CMyStackWalker.h"

initialiseSingleton(CLogicDriver);


void CLogicDriver::Loop()
{
	LoopGetPacketStream();
}

void CLogicDriver::LoopGetPacketStream()
{
	char Buffer[name_msg_packet::SOCKET_BUFF_SIZE];

	while(1)
	{
		memset(Buffer, 0, sizeof(Buffer));
		if ( 0!=net::GetPacketStream(Buffer, sizeof(Buffer)) )
		{
			break;
		}

		IPackHead *pPackHead = (IPackHead*)Buffer;
		IPacketObject *pPackObject = g_PacketFactory.GetPacketObject(pPackHead);
		if(NULL==pPackHead)
		{
			LOGE("CLogicDriver::LoopGetPacketStream,id:%d\n", pPackObject->GetPacketID());
			continue;
		}

		__try
		{
			pPackObject->Execute(pPackHead);
		}
		__except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))
		{

		}
	}
}
