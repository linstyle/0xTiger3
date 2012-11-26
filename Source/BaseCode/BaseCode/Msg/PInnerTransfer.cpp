#include "CNetKernel.h"
#include "PInnerTransfer.h"

initialiseSingleton(POInnerTransfer);

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

P_INNER_TRANSFER_ERR* PInnerTransfer::GetErrInnerTransferPacket()
{
	return m_pErrInnerTransfer;
}


bool PInnerTransfer::CreateNtoL(unsigned int nNetKey)
{
	m_pInnerTransfer->m_nPacketDefine1 = PACKET1_INNER_NET_LOGIC_QUEUE;
	m_pInnerTransfer->m_nPacketDefine2 = D2_INNER_N_TO_L_SEND;
	m_pInnerTransfer->m_nPacketSize = sizeof(P_INNER_TRANSFER);

	m_pInnerTransfer->m_nNetKey = nNetKey;

	return true;
}

bool PInnerTransfer::CreateNtoLErr(unsigned int nNetKey)
{
	m_pErrInnerTransfer->m_nPacketDefine1 = PACKET1_INNER_NET_LOGIC_QUEUE;
	m_pErrInnerTransfer->m_nPacketDefine2 = D2_INNER_N_TO_L_ERR;
	m_pErrInnerTransfer->m_nPacketSize = sizeof(P_INNER_TRANSFER_ERR);

	m_pErrInnerTransfer->m_nNetKey = nNetKey;	

	return true;
}

bool PInnerTransfer::CreateLtoN(unsigned int nNetKey, const char* pLogicPacket, unsigned short nLogicPacketSize)
{
	if (nLogicPacketSize > NET_PACKET_BUFF_SIZE)
	{
		return false;
	}

	m_pInnerTransfer->m_nPacketDefine1 = PACKET1_INNER_NET_LOGIC_QUEUE;
	m_pInnerTransfer->m_nPacketDefine2 = D2_INNER_L_TO_N_SEND;
	m_pInnerTransfer->m_nPacketSize = sizeof(P_INNER_TRANSFER);

	m_pInnerTransfer->m_nNetKey = nNetKey;
	memcpy(GetPacketBuffer(), pLogicPacket, nLogicPacketSize);

	return true;
}

bool PInnerTransfer::CreateLtoNErr(unsigned int nNetKey)
{
	m_pErrInnerTransfer->m_nPacketDefine1 = PACKET1_INNER_NET_LOGIC_QUEUE;
	m_pErrInnerTransfer->m_nPacketDefine2 = D2_INNER_L_TO_N_ERR;
	m_pErrInnerTransfer->m_nPacketSize = sizeof(P_INNER_TRANSFER_ERR);

	m_pErrInnerTransfer->m_nNetKey = nNetKey;

	return true;
}


/****************************
	IPacketObject
*****************************/
void POInnerTransfer::Execute(IPacketHead* pPacketHead)
{
	IFn(!pPacketHead)
		return;

	PInnerTransfer* pPInnerTransfer = (PInnerTransfer*)pPacketHead;
	IFn(!pPInnerTransfer)
		return;

	CNetKernel* pNetKernel = g_NetDriver2.GetNetKerner();
	IFn(!pNetKernel)
		return; 

	switch(pPacketHead->GetPacketDefine2())
	{
	case D2_INNER_N_TO_L_SEND:
		g_PacketFactory.ProcessMsg( (IPacketHead*)(pPInnerTransfer->GetInnerTransferPacket()) );
		break;

	case D2_INNER_N_TO_L_ERR:
		break;

	case D2_INNER_L_TO_N_SEND:
		pNetKernel->SendToBufferByNetKey( (IPacketHead*)(pPInnerTransfer->GetInnerTransferPacket()), 
			pPInnerTransfer->GetNetKey());
		break;

	case D2_INNER_L_TO_N_ERR:
		pNetKernel->CloseClientSocketByNetKey(pPInnerTransfer->GetNetKey());
		break;

	default:
		LOGE("POInnerTransfer::Execute.PacketDefin2:%d\n", pPacketHead->GetPacketDefine2());
	}
}