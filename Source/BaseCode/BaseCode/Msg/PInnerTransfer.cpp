#include "CNetKernel.h"
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

unsigned int PInnerTransfer::GetNetKey()
{
	return m_pInnerTransfer->m_nNetKey;
}

P_INNER_TRANSFER* PInnerTransfer::GetInnerTransferPacket()
{
	return m_pInnerTransfer;
}


bool PInnerTransfer::CreateNtoL(unsigned int nNetKey, const char* pNetPacket, unsigned short nNetPacketSize)
{
	if (nNetPacketSize > NET_PACKET_BUFF_SIZE)
	{
		return false;
	}

	m_pInnerTransfer->m_nPacketDefine1 = PACKET1_INNER_NET_LOGIC_QUEUE;
	m_pInnerTransfer->m_nPacketDefine2 = D2_INNER_N_TO_L_SEND;
	m_pInnerTransfer->m_nPacketSize = sizeof(P_INNER_TRANSFER);

	m_pInnerTransfer->m_nNetKey = nNetKey;

	return true;
}

bool PInnerTransfer::CreateNtoLErr(unsigned int nNetKey)
{
	m_pInnerTransfer->m_nPacketDefine1 = PACKET1_INNER_NET_LOGIC_QUEUE;
	m_pInnerTransfer->m_nPacketDefine2 = D2_INNER_N_TO_L_ERR;
	m_pInnerTransfer->m_nPacketSize = sizeof(P_INNER_TRANSFER_ERR);

	m_pInnerTransfer->m_nNetKey = nNetKey;	

	return true;
}

bool PInnerTransfer::CreateLtoN(unsigned int nNetKey, const char* pNetPacket, unsigned short nNetPacketSize)
{
	if (nNetPacketSize > NET_PACKET_BUFF_SIZE)
	{
		return false;
	}

	m_pInnerTransfer->m_nPacketDefine1 = PACKET1_INNER_NET_LOGIC_QUEUE;
	m_pInnerTransfer->m_nPacketDefine2 = D2_INNER_L_TO_N_SEND;
	m_pInnerTransfer->m_nPacketSize = sizeof(P_INNER_TRANSFER);

	m_pInnerTransfer->m_nNetKey = nNetKey;

	return true;
}

bool PInnerTransfer::CreateLtoNErr(unsigned int nNetKey)
{
	m_pInnerTransfer->m_nPacketDefine1 = PACKET1_INNER_NET_LOGIC_QUEUE;
	m_pInnerTransfer->m_nPacketDefine2 = D2_INNER_L_TO_N_ERR;
	m_pInnerTransfer->m_nPacketSize = sizeof(P_INNER_TRANSFER_ERR);

	m_pInnerTransfer->m_nNetKey = nNetKey;

	return true;
}


/****************************
	IPacketObject
*****************************/
POInnerTransfer g_POInnerTransfer;

void POInnerTransfer::Execute(IPacketHead* pPackHead)
{
	IFn(!pPackHead)
		return;

	PInnerTransfer* pPInnerTransfer = (PInnerTransfer*)pPackHead;
	IFn(!pPInnerTransfer)
		return;

	CNetKernel* pNetKernel = g_NetDriver2.GetNetKerner();
	IFn(!pNetKernel)
		return; 

	bool bResult = false;
	switch(pPackHead->GetPacketDefine2())
	{
	case D2_INNER_N_TO_L_SEND:

		break;

	case D2_INNER_N_TO_L_ERR:
		break;

	case D2_INNER_L_TO_N_SEND:
		bResult = pNetKernel->SendToBufferByNetKey( (IPacketHead*)(pPInnerTransfer->GetInnerTransferPacket()), 
			pPInnerTransfer->GetNetKey());
		break;

	case D2_INNER_L_TO_N_ERR:
		bResult = true;
		pNetKernel->CloseClientSocketByNetKey(pPInnerTransfer->GetNetKey());
		break;

	default:
		LOGE("POInnerTransfer::Execute.PacketDefin2:%d\n", pPackHead->GetPacketDefine2());
		bResult = true;
	}

	if (!bResult)
	{
		LOGE("POInnerTransfer::Execute(IPacketHead* pPackHead) return false.PacketDefin2:%d\n", 
			pPackHead->GetPacketDefine2());
	}

}