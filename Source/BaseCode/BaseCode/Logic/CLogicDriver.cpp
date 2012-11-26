#include "CLogicDriver.h"
#include "CLog2.0.h"
#include "CNLBridgeQueue.h"
#include "GlobalFunction.h"
#include "CPacketFactory.h"

initialiseSingleton(CLogicDriver);

void CLogicDriver::Loop()
{
	char BufferPacket[NET_PACKET_BUFF_SIZE];
	const char* pBuffer = BufferPacket;

	while (1)
	{
		MEMSET(&BufferPacket, 0, sizeof(BufferPacket));
		if ( !g_NLBridgeQueue.GetFromNetQueue(BufferPacket, NET_PACKET_BUFF_SIZE) )
		{
			break;
		}

		IPacketHead* pPacketHead = (IPacketHead*)pBuffer;

		g_PacketFactory.ProcessMsg(pPacketHead);

	}	
}