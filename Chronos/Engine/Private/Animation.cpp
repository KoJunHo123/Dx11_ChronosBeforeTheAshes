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

_bool CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool isLoop, _float fTimeDelta)
{
	if (m_CurrentTrackPosition == 0.)
		m_isFinished = false;

 	m_CurrentTrackPosition += m_SpeedPerSec * fTimeDelta;

	if (m_CurrentTrackPosition >= m_Duration && false == m_isFinished)	// 현재 트랙 위치가 최대를 넘으면
	{
		m_CurrentTrackPosition = 0.;
		m_isFinished = true;
		if (false == isLoop)	// 루프 안할거면 반환
		{
			for (_uint& iIndex : m_CurrentKeyFrameIndices)
				iIndex = 0;
			return true;
		}
	}

	_uint		iChannelIndex = { 0 };
	_vector vTranslation = XMLoadFloat4(&m_vPreTranslation);
	for(auto& pChannel : m_Channels)
		pChannel->Update_TransformationMatrix(Bones, &m_CurrentKeyFrameIndices[iChannelIndex++], m_CurrentTrackPosition);
	XMStoreFloat4(&m_vPreTranslation, vTranslation);

	return false;
}

_bool CAnimation::Update_ChangeAnimation(CAnimation* pAnimation, const vector<class CBone*>& Bones, _float fTimeDelta, _float& m_fChangeRate)
{
	if (0. == m_CurrentTrackPosition && true == m_isFinished)
	{
		//m_CurrentKeyFrameIndices : 이거는 각 채널당 키프레임의 현재 인덱스의 배열.
		_uint iIndex = m_Channels[0]->Get_KeyFrameSize() - 1;

		for (auto& elem : m_CurrentKeyFrameIndices)
			elem = iIndex;

		m_CurrentTrackPosition = (_double)m_CurrentKeyFrameIndices[0];
	}

	m_CurrentTrackPosition += m_SpeedPerSec * fTimeDelta;

	_bool isChanged = false;
	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		isChanged = m_Channels[i]->Update_ChangeChannel(pAnimation->m_Channels[i], Bones, &m_CurrentKeyFrameIndices[i], m_CurrentTrackPosition);

		if (1 == m_Channels[i]->Get_BoneIndex())
			m_fChangeRate = m_Channels[i]->Get_Ratio();
	}

	if (true == isChanged)
	{
		m_CurrentTrackPosition = 0.;
		for (_uint& iIndex : m_CurrentKeyFrameIndices)
			iIndex = 0;
	}

	return isChanged;
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
