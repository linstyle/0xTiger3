/*  作者：		林东平                                                              
	创建时间：	2010/08/20                                                        
    功能说明：  网络DEBUG记录
*/

#pragma once

#include "GlobalType.h"
#include "CLock.h"

class CNetTracer
{
public:
	CNetTracer()
	{
		/*
			m_nAccept;		当前连接数	
			m_nAcceptErr;	错误的连接数
			m_nConnectDis;	断连的次数
			m_nConnectErr;	错误的连接次数
		*/
		m_nAccept = m_nAcceptErr = m_nConnectDis = m_nConnectErr = 0;
		m_nRecv = m_nSend = m_nRecvSec = m_nSendSec = 0;
	}

	CAtom m_nAccept;
	CAtom m_nAcceptErr;
	CAtom m_nConnectDis;
	CAtom m_nConnectErr;
	CAtom m_nRecv;
	CAtom m_nSend;	
	/*
		每秒计数
	*/
	CAtom m_nRecvSec;
	CAtom m_nSendSec;

	void Tracer()
	{
		DELAY_CONTINUE(5000)
		{
			printf("Notice:AC:%d, AErr:%d, CDis:%d, CErr:%d, RecvS:%d, Sends:%d\n",
				m_nAccept.GetValue(), m_nAcceptErr.GetValue(), m_nConnectDis.GetValue(),
				m_nConnectErr.GetValue(), m_nRecv.GetValue(), m_nSend.GetValue());		
		}

		TracerSec();;
	}

	void TracerSec()
	{	
		DELAY_CONTINUE(1000)
		{
			printf("Notice:RecvSec:%d, SendSec:%d\n", m_nRecvSec.GetValue(), m_nSendSec.GetValue());
			m_nRecvSec = 0;
			m_nSendSec = 0;	
		}
	}
};

extern CNetTracer g_NetTracer;