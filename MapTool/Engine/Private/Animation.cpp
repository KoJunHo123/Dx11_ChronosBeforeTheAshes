#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(ifstream* infile)
{
	infile->read(reinterpret_cast<_char*>(&m_Duration), sizeof(_double));
	infile->read(reinterpret_cast<_char*>(&m_SpeedPerSec), sizeof(_double));
	infile->read(reinterpret_cast<_char*>(&m_iNumChannels), sizeof(_uint));

	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(infile);

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

CAnimation* CAnimation::Create(ifstream* infile)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(infile)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	__super::Free();
	for (auto& channel : m_Channels)
		Safe_Release(channel);
}
