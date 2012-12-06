#include "CBasePlayer.h"

CBasePlayer::CBasePlayer()
{
	m_nNetKey = 0;
}

unsigned int CBasePlayer::GetNetKey()
{
	return m_nNetKey;
}

void CBasePlayer::SetNetKey(unsigned int nNetKey)
{
	m_nNetKey = nNetKey;
}