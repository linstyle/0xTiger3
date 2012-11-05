#include "stdafx.h"
#include "CBaseLogicPlayer.h"

CBaseLogicPlayer::CBaseLogicPlayer()
{
	m_nUserID = 0;
}

CBaseLogicPlayer::~CBaseLogicPlayer()
{

}

INT64 CBaseLogicPlayer::GetUserID()
{
	return m_nUserID;
}

void CBaseLogicPlayer::SetUserID(INT64 nUserID)
{
	m_nUserID = nUserID;
}