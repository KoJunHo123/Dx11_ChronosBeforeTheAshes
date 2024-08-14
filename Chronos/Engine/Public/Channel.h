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
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _double CurrentTrackPosition);
	_bool Update_ChangeChannel(CChannel* pChannel, const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _double CurrentTrackPosition);

	_uint Get_BoneIndex() {
		return m_iBoneIndex;
	}

private:
	_char m_szName[MAX_PATH] = {};

	_uint m_iNumKeyFrames = { 0 };
	_uint m_iBoneIndex = { 0 };
	vector<KEYFRAME> m_KeyFrames;


public:
	static CChannel* Create(ifstream* infile, const class CModel* pModel);
	virtual void Free();
};
END
