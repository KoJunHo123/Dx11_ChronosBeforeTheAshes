#pragma once

#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize(_uint iMaxChannel);

public:
	void SoundPlay(TCHAR* pSoundKey, _uint iChannelID, _float fVolume);
	void PlayBGM(TCHAR* pSoundKey, _uint iBGMChannel, _float fVolume);
	void StopSound(_uint iChannelID);
	void StopAll();
	void SetChannelVolume(_uint iChannelID, _float fVolume);
	void LoadSoundFile();

private:
	// 사운드 리소스 정보를 갖는 객체
	map<TCHAR*, Sound*>	m_mapSound;

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	System*				m_pSystem = { nullptr };

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	vector<Channel*>	m_vecChannel;
	_uint				m_iVersion = {};

public:
	static CSound_Manager* Create(_uint iMaxChannel);
	virtual void Free() override;
};

END