#pragma once

#include "Player_State.h"

BEGIN(Client)
class CPlayer_Attack final : public CPlayer_State
{
	enum PLAYER_ATTACK_STATE{ ATTACK_LIGHT,
		ATTACK_POWER_CHARGE, ATTACK_POWER,
		ATTACK_BACK, ATTACK_RUN, ATTACK_DODGE,
		ATTACK_END
	};
private:
	CPlayer_Attack();
	virtual ~CPlayer_Attack() = default;

public:
	HRESULT Initialize(void* pArg) override;
	HRESULT StartState(void** pArg) override;
	void Priority_Update(_float fTimeDelta) override;
	void Update(_float fTimeDelta) override;
	void Late_Update(_float fTimeDelta) override;
	HRESULT Render() override;
	HRESULT ExitState(void** pArg) override;

private:
	void Light_Attack();
	void Power_Attack();

private:
	PLAYER_ATTACK_STATE m_eAttackState = { ATTACK_END };

	_float m_fAttackDelay = { 0.f };
	
	// 몇번째 공격인지
	_uint m_iAttackCount = 0;
	// 약공격 최대 개수
	_uint m_iMaxLight = 4;
	// 강공격 최대 개수
	_uint m_iMaxPower = 2;


public:
	static CPlayer_Attack* Create(void* pArg);
	virtual void Free();
};
END
