#pragma once

#include "Player_State.h"

BEGIN(Client)
class CPlayer_Jump final : public CPlayer_State
{
public:
	typedef struct : CPlayer_State::PLAYER_STATE_DESC
	{
		_float* pHP = { nullptr };
	}PLAYER_STATE_JUMP_DESC;

private:
	CPlayer_Jump();
	virtual ~CPlayer_Jump() = default;

public:
	HRESULT Initialize(void* pArg) override;
	HRESULT StartState(void** pArg) override;
	void Priority_Update(_float fTimeDelta) override;
	void Update(_float fTimeDelta) override;
	void Late_Update(_float fTimeDelta) override;
	HRESULT Render() override;
	HRESULT ExitState(void** pArg) override;

private:
	_float* m_pHP = { nullptr };

private:
	_float m_fSpeed = { 0.f };

public:
	static CPlayer_Jump* Create(void* pArg);
	virtual void Free();
};
END
