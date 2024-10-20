#pragma once

#include "Player_State.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CPlayer_Action final : public CPlayer_State
{
public:
	typedef struct : CPlayer_State::PLAYER_STATE_DESC
	{
		class CCollider* pPlayerColliderCom = { nullptr };
		_bool* pItemUsed = { nullptr };
	}PLAYER_ACTION_DESC;

	enum PLAYER_ACTION_STATE { STATE_DRAGONHEART, STATE_DRAGONSTONE, STATE_TELEPORT, STATE_WAYPOINT, STATE_REVIVE, STATE_RUNEKEY, STATE_END };

private:
	CPlayer_Action();
	virtual ~CPlayer_Action() = default;

public:
	void Set_State(_uint iStateIndex) {
		m_eState = (PLAYER_ACTION_STATE)iStateIndex;
	}

	void Set_TargetPosition(_fvector vPosition) {
		XMStoreFloat3(&m_vTargetPosition, vPosition);
	}

public:
	HRESULT Initialize(void* pArg) override;
	HRESULT StartState(void** pArg) override;
	void Priority_Update(_float fTimeDelta) override;
	void Update(_float fTimeDelta) override;
	void Late_Update(_float fTimeDelta) override;
	HRESULT Render() override;
	HRESULT ExitState(void** pArg) override;

private:
	class CCollider* m_pPlayerColliderCom = { nullptr };
	PLAYER_ACTION_STATE m_eState = { STATE_END };
	_float3 m_vTargetPosition = {};
	_bool* m_pItemUsed = { nullptr };
	
	_bool m_bTeleportStart = { false };
	_bool m_bFadeIn = { false };
	_bool m_bFadeOut = { false };

	_bool m_bLeftGrab = { false };
	_bool m_bRightGrab = { false };

private:
	HRESULT Add_Particle_Teleport();
	HRESULT Add_TrailRevolve();

public:
	static CPlayer_Action* Create(void* pArg);
	virtual void Free();
};
END
