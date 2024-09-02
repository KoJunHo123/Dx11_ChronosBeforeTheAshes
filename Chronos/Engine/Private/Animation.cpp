#include "Animation.h"
#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Prototype)
	: m_iNumChannels{ Prototype.m_iNumChannels }
	, m_Channels{ Prototype.m_Channels }
	, m_Duration{ Prototype.m_Duration }
	, m_SpeedPerSec{ Prototype.m_SpeedPerSec }
	, m_CurrentTrackPosition{ Prototype.m_CurrentTrackPosition }
	, m_CurrentKeyFrameIndices{ Prototype.m_CurrentKeyFrameIndices }
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(ifstream* infile, const class CModel* pModel, _uint iBoneNum)
{
	infile->read(reinterpret_cast<_char*>(&m_Duration), sizeof(_double));
	infile->read(reinterpret_cast<_char*>(&m_SpeedPerSec), sizeof(_double));
	infile->read(reinterpret_cast<_char*>(&m_iNumChannels), sizeof(_uint));

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(infile, pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.emplace_back(pChannel);
	}

	return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool isLoop, _float fTimeDelta, _bool bChangeStart, _bool& isChange)
{
 	m_CurrentTrackPosition += m_SpeedPerSec * fTimeDelta;

	_double RatioMax = 5.;

	if (1. == m_Duration)
	{
		if (false == isChange)
			m_CurrentTrackPosition = 0.;
		else
			RatioMax = 1.;
	}

	if (true == isChange)
	{
		if (true == bChangeStart)
		{
			m_CurrentTrackPosition = 0.;
		}

		if (m_CurrentTrackPosition > RatioMax)
		{
			m_CurrentTrackPosition = 0.;
			isChange = false;
		}
	}
	else if (m_CurrentTrackPosition >= m_Duration)	// 현재 트랙 위치가 최대를 넘으면
	{
		m_CurrentTrackPosition = 0.;
		if (false == isLoop)	// 루프 안할거면 반환
		{
			for (_uint& iIndex : m_CurrentKeyFrameIndices)
				iIndex = 0;
			return true;
		}
	}

	_uint		iChannelIndex = { 0 };
	_vector		vTranslation = XMLoadFloat4(&m_vPreTranslation);

	for(auto& pChannel : m_Channels)
		pChannel->Update_TransformationMatrix(Bones, &m_CurrentKeyFrameIndices[iChannelIndex++], m_CurrentTrackPosition, isChange, RatioMax);

	XMStoreFloat4(&m_vPreTranslation, vTranslation);

	return false;
}

_uint CAnimation::Find_RootBoneIndex()
{
	_uint iRootBoneIndex = 0;
	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		if (1 == m_Channels[i]->Get_BoneIndex())
			iRootBoneIndex = i;
	}

	return iRootBoneIndex;
}

CAnimation* CAnimation::Create(ifstream* infile, const class CModel* pModel, _uint iBoneNum)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(infile, pModel, iBoneNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	__super::Free();
	for (auto& channel : m_Channels)
		Safe_Release(channel);
}
