/*
	@lindp lin_style@foxmail.com   
	@2012/12/6
	@�ṩһ��nNetKey�Ľӿڻ���
*/

#pragma once

class CBasePlayer
{
public:
	CBasePlayer();

	unsigned int GetNetKey();
	void SetNetKey(unsigned int nNetKey);

private:


public:

private:
	unsigned int m_nNetKey;

};