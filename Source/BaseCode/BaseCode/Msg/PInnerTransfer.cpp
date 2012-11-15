#include "PInnerTransfer.h"

PInnerTransfer::PInnerTransfer()
{
	//将指针悬挂到基类的BUFFER
	m_pUnionBuffer = GetPacketBuffer();

	//检查下大小是否超出(检查与否都可以,静态的结构不可能超出PACKET_BUFF_SIZE)
	IFn(sizeof(P_INNER_TRANSFER) > NET_PACKET_BUFF_SIZE )
	{
	
	}
	
}

void PInnerTransfer::Process()
{
	switch(GetPacketDefine2())
	{
	case D2_INNER_N_TO_L:
		break;

	case D2_INNER_L_TO_N:
		break;

	default:
		LOGNW("PInnerTransfer::Process().switch default.PacketDefine2:%d\n", GetPacketDefine2());
	}
}


bool PInnerTransfer::CreateNtoL(unsigned int nNetKey, const char* pNetPacket, unsigned short nNetPacketSize)
{
	if (nNetPacketSize > NET_PACKET_BUFF_SIZE)
	{
		return false;
	}

	m_pInnerTransfer->m_nPacketDefine1 = PACKET1_INNER_NET_LOGIC;
	m_pInnerTransfer->m_nPacketDefine2 = D2_INNER_N_TO_L;
	m_pInnerTransfer->m_nPacketSize = sizeof(P_INNER_TRANSFER);

	m_pInnerTransfer->m_nNetKey = nNetKey;

	return true;
}