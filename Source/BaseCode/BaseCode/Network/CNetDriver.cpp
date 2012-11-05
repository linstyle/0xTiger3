#include "StdAfx.h"
#include "CNetDriver.h"

initialiseSingleton(CNetDriver);

void CNetDriver::TracerMe()
{
	DELAY_CONTINUE(1000*5)
	Tracer();
}

void CNetDriver::Tracer()
{
	printf("	Notic: CNetDriver::TracerMe\n");
	m_SocketRun.TracerMe();
}

void CNetDriver::Init(ILocalObjectConnect* pObjectConnect, int nConnectCount, 
					  ILocalObjectAccept* pObjectAccept, int nAcceptCount)
{
	m_SocketRun.Init(pObjectConnect, nConnectCount, pObjectAccept, nAcceptCount);
}

void CNetDriver::Run()
{
	m_SocketRun.Run();
}

int CNetDriver::Send(CIOCPData *pIOCPKey, IPackHead *pPackHead)
{
	return m_SocketRun.Send(pIOCPKey, pPackHead);
}