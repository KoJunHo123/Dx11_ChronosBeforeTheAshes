#pragma once

#include "../Public/Base.h"

BEGIN(Engine)

class CKeyManager final : public CBase
{
private:
	CKeyManager();
	virtual ~CKeyManager() = default;

public:
	HRESULT Initialize();

public:
	_bool	Key_Pressing(int _iKey);
	_bool	Key_Down(int _iKey);
	_bool	Key_Up(int _iKey);

private:
	_bool	m_bKeyState[VK_MAX] = {};

public:
	static CKeyManager* Create();
	virtual void Free();
};

END