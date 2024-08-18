#include "../Public/Sound_Manager.h"

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize(_uint iMaxChannel)
{
	if (FMOD_OK != System_Create(&m_pSystem))
		return E_FAIL;

	if (FMOD_OK != m_pSystem->getVersion(&m_iVersion))
		return E_FAIL;

	if (FMOD_OK != m_pSystem->init(32, FMOD_INIT_NORMAL, nullptr))
		return E_FAIL;

	m_vecChannel.resize(iMaxChannel, nullptr);

	return S_OK;
}

void CSound_Manager::SoundPlay(TCHAR* pSoundKey, _uint iChannelID, _float fVolume)
{
	auto iter = m_mapSound.find(pSoundKey);
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	_bool isPlaying = false;

	if (m_vecChannel[iChannelID]->isPlaying(&isPlaying))
	{
		m_vecChannel[iChannelID]->stop();
		m_pSystem->playSound(iter->second, 0, false, &m_vecChannel[iChannelID]);
	}

	m_vecChannel[iChannelID]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound_Manager::PlayBGM(TCHAR* pSoundKey, _uint iBGMChannel, _float fVolume)
{
	auto iter = m_mapSound.find(pSoundKey);
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	m_pSystem->playSound(iter->second, 0, false, &m_vecChannel[iBGMChannel]);
	m_vecChannel[iBGMChannel]->setMode(FMOD_LOOP_NORMAL);
	m_vecChannel[iBGMChannel]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound_Manager::StopSound(_uint iChannelID)
{
	m_vecChannel[iChannelID]->stop();
}

void CSound_Manager::StopAll()
{
	for (auto iter : m_vecChannel)
		iter->stop();
}

void CSound_Manager::SetChannelVolume(_uint iChannelID, _float fVolume)
{
	m_vecChannel[iChannelID]->setVolume(fVolume);
}

void CSound_Manager::LoadSoundFile()
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddatai64_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirsti64("../../Client/Resources/Sound/*.wav", &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	// D:\유준환\145기\DefaultWindow\Sound	: 절대 경로

	char szCurPath[128] = "../../Client/Resources/Sound/";	 // 상대 경로
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../Sound/" + "Success.wav"
		strcat_s(szFullPath, fd.name);
		// "../Sound/Success.wav"

		Sound* pSound = nullptr;

		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_LOOP_OFF, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = (int)strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnexti64(handle, &fd);
	}

	m_pSystem->update();

	_findclose(handle);
}

CSound_Manager* CSound_Manager::Create(_uint iMaxChannel)
{
	CSound_Manager* pInstance = new CSound_Manager();
	if (FAILED(pInstance->Initialize(iMaxChannel)))
	{
		MSG_BOX(TEXT("Failed to Created : CSound_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	__super::Free();

	m_vecChannel.clear();

	for (auto& MyPair : m_mapSound)
	{
		delete[] MyPair.first;
		MyPair.second->release();
	}
	m_mapSound.clear();

	m_pSystem->release();
	m_pSystem->close();
}
