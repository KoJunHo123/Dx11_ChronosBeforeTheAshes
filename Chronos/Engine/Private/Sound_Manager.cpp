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
	m_SoundDescs.resize(iMaxChannel);

	m_iMaxChannel = iMaxChannel;

	return S_OK;
}

void CSound_Manager::Update(_float fTimeDelta)
{
	for (auto& iter = m_StopSoundIndex.begin(); iter != m_StopSoundIndex.end(); )
	{
		_float fVolume = { 0.f };
		m_vecChannel[*iter]->getVolume(&fVolume);

		fVolume -= fTimeDelta;
		m_vecChannel[*iter]->setVolume(fVolume);

		if (fVolume < 0.f)
		{
			m_vecChannel[*iter]->stop();
			iter = m_StopSoundIndex.erase(iter);
		}
		else
			++iter;
	}

	auto& SoundIter = m_SoundDescs.begin();
	_uint iIndex = { 0 };

	for (auto& iter = m_vecChannel.begin(); iter != m_vecChannel.end();)
	{
		if (nullptr == *iter || m_iBGMIndex == iIndex)
		{
			++iter;
			++SoundIter;
			++iIndex;
			continue;
		}

		_vector vCenterPos = XMLoadFloat3(&m_vCenter);
		_vector vSoundPos = XMLoadFloat3(&SoundIter->vPos);

		_float fMaxDistance = SoundIter->fMaxDistance;
		_float fLength = XMVectorGetX(XMVector3Length(vCenterPos - vSoundPos));

		_float fRatio = 1.f - (fLength / fMaxDistance);

		if (0.f == fMaxDistance)
			fRatio = 1.f;
		else if (fRatio < 0.f)
			fRatio = 0.f;

		(*iter)->setVolume(SoundIter->fVolume * fRatio);

		if (iIndex < m_iMaxChannel)
		{
			++iIndex;
			++iter;
			++SoundIter;
			continue;
		}

		_bool isPlaying = { false };
		(*iter)->isPlaying(&isPlaying);
		if (false == isPlaying)
		{
			iter = m_vecChannel.erase(iter);	
			SoundIter = m_SoundDescs.erase(SoundIter);
		}
		else
		{
			++iter;
			++SoundIter;
		}

		++iIndex;
	}
	
	
}

void CSound_Manager::Set_Center(_float3 vCenter)
{
	m_vCenter = vCenter;
}

void CSound_Manager::SoundPlay(TCHAR* pSoundKey, _uint iChannelID, const SOUND_DESC& desc)
{
	auto iter = m_mapSound.find(pSoundKey);
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if(nullptr != m_vecChannel[iChannelID])
	{
		_bool isPlaying = false;
		m_vecChannel[iChannelID]->isPlaying(&isPlaying);
		if (true == isPlaying)
		{
			m_vecChannel[iChannelID]->stop();
		}
	}

	m_pSystem->playSound(iter->second, 0, false, &m_vecChannel[iChannelID]);

	m_vecChannel[iChannelID]->setVolume(desc.fVolume);
	m_pSystem->update();

	m_SoundDescs[iChannelID] = desc;
}

void CSound_Manager::SoundPlay_Additional(TCHAR* pSoundKey, const SOUND_DESC& desc)
{
	auto iter = m_mapSound.find(pSoundKey);
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	Channel* pChannel = { nullptr };
	m_pSystem->playSound(iter->second, 0, false, &pChannel);
	pChannel->setVolume(desc.fVolume);
	m_vecChannel.emplace_back(pChannel);
	m_pSystem->update();

	m_SoundDescs.emplace_back(desc);
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

	m_iBGMIndex = iBGMChannel;
}

void CSound_Manager::StopSound(_uint iChannelID)
{
	m_vecChannel[iChannelID]->stop();
}

void CSound_Manager::StopSoundSlowly(_uint iChannelID)
{
	m_StopSoundIndex.emplace_back(iChannelID);
}

void CSound_Manager::StopAll()
{
	for (auto iter : m_vecChannel)
		iter->stop();
}

void CSound_Manager::Set_ChannelVolume(_uint iChannelID, _float fVolume)
{
	m_vecChannel[iChannelID]->setVolume(fVolume);
}

void CSound_Manager::LoadSoundFile()
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddatai64_t fd;
	
	path strFilePath = "../../Client/Bin/Resources/SFX/";
	for (auto& entry : recursive_directory_iterator(strFilePath))
	{
		if (is_directory(entry.path())) {
			bool has_subdirectories = false;

			// 하위 항목 순회 (하위 폴더가 있는지 확인)
			for (const auto& sub_entry : directory_iterator(entry.path())) {
				if (is_directory(sub_entry.path())) {
					has_subdirectories = true; // 하위 폴더가 있으면 true
					break; // 하위 폴더를 하나라도 찾으면 더 이상 확인할 필요 없음
				}
			}

			// 하위 폴더가 없으면 리프 폴더로 간주하고 출력
			if (!has_subdirectories) {
				std::cout << entry.path() << std::endl;
				string strFilePath = entry.path().string();
				strFilePath += "/*.ogg";

				// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
				intptr_t handle = _findfirsti64(strFilePath.c_str(), &fd);

				if (handle == -1)
					continue;

				int iResult = 0;

				// D:\유준환\145기\DefaultWindow\SFX	: 절대 경로

				string strPath = entry.path().string();
				strPath += "/";
				char szCurPath[128] = "";	 // 상대 경로
				strcpy_s(szCurPath, strPath.c_str());
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
		}
	}
}

_uint CSound_Manager::Get_Position(_uint iChannelID)
{
	_uint iPosition = { 0 };
	m_vecChannel[iChannelID]->getPosition(&iPosition, FMOD_TIMEUNIT_MS);
	return iPosition;
}

void CSound_Manager::Set_Position(_uint iChannelID, _uint iPositionMS)
{
	m_vecChannel[iChannelID]->setPosition(iPositionMS, FMOD_TIMEUNIT_MS);
}

_bool CSound_Manager::IsPlaying(_uint iChannelID)
{
	if (nullptr == m_vecChannel[iChannelID])
		return false;

	_bool isPlaying = { false };
	m_vecChannel[iChannelID]->isPlaying(&isPlaying);
	return isPlaying;
}

void CSound_Manager::Set_Frequency(_uint iChannelID, _float fFrequency)
{
	_float fCurrentFrequency = { 0.f };
	m_vecChannel[iChannelID]->getFrequency(&fCurrentFrequency);
	m_vecChannel[iChannelID]->setFrequency(fCurrentFrequency * fFrequency);
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
