#pragma once

#include "Base.h"

BEGIN(Engine)
class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(ifstream* infile, const class CModel* pModel);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _double CurrentTrackPosition, _bool isChange, _double RatioMax);

	_uint Get_BoneIndex() {
		return m_iBoneIndex;
	}

	_uint Get_KeyFrameSize() {
		return (_uint)m_KeyFrames.size();
	}

	_float Get_Ratio() {
		return (_float)m_Ratio;
	}

private:
	_char m_szName[MAX_PATH] = {};

	_uint m_iNumKeyFrames = { 0 };
	_uint m_iBoneIndex = { 0 };
	vector<KEYFRAME> m_KeyFrames;

	_double m_Ratio = { 0.f };

	_float4 m_vSourScale{}, m_vSourRotation{}, m_vSourTranslation{};

public:
	static CChannel* Create(ifstream* infile, const class CModel* pModel);
	virtual void Free();
};
END
