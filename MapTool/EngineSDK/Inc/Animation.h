#pragma once

#include "Base.h"
BEGIN(Engine)
class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;
public:
	HRESULT Initialize(ifstream* infile, const class CModel* pModel, _uint iBoneNum);
	_bool Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool isLoop, _float fTimeDelta);
	_bool Update_ChangeAnimation(CAnimation* pAnimation , const vector<class CBone*>& Bones,_float fTimeDelta);

private:
	/* 이 애니메이션을 구동하기위해 걸리는 전체 거리. */
	_double m_Duration = { 0 };
	/* 애니메이션의 초당 재생속도 */
	_double m_SpeedPerSec = { 0 };
	/* m_CurrentTrackPosition += m_SpeedPerSec * fTimeDelta */
	_double m_CurrentTrackPosition = { 0 };
	/* 이 애니메이션이 사용하는 뼈의 갯수 */
	_uint m_iNumChannels = { 0 };

	_float4 m_vPreTranslation = {};

	vector<_uint>				m_CurrentKeyFrameIndices = { 0 };
	vector<class CChannel*>		m_Channels;

public:
	static CAnimation* Create(ifstream* infile, const class CModel* pModel, _uint iBoneNum);
	CAnimation* Clone();
	virtual void Free() override;
};
END
