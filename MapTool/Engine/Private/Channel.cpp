#include "Channel.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(ifstream* infile)
{
	infile->read(reinterpret_cast<_char*>(m_szName), sizeof(_char) * MAX_PATH);
	infile->read(reinterpret_cast<_char*>(&m_iNumKeyFrames), sizeof(_uint));
	
	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};
		infile->read(reinterpret_cast<_char*>(&KeyFrame), sizeof(KEYFRAME));
		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

CChannel* CChannel::Create(ifstream* infile)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(infile)))
	{
		MSG_BOX(TEXT("Failed To Create : CChannel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
	__super::Free();
}
