#pragma once

#include "Base.h"

BEGIN(Engine)
class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pAIChannel, ofstream* outfile);

private:
	_char				m_szName[MAX_PATH] = {};

	_uint				m_iNumKeyFrames = {};
	vector<KEYFRAME>	m_KeyFrames;

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, ofstream* outfile);
	virtual void Free() override;
};
END
