#pragma once

#include "Base.h"
BEGIN(Engine)
class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;
public:
	HRESULT Initialize(ifstream* infile);

private:
	_double m_Duration = { 0 };
	_double m_SpeedPerSec = { 0 };
	_uint m_iNumChannels = { 0 };

	vector<class CChannel*> m_Channels;

public:
	static CAnimation* Create(ifstream* infile);
	virtual void Free() override;
};
END
