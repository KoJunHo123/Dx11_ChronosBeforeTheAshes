#pragma once

#include "Player_State.h"

BEGIN(Client)
class CPlayer_Move final : public CPlayer_State
{
public:
	typedef struct : CPlayer_State::PLAYER_STATE_DESC
	{
		_bool* pNonIntersect = { nullptr };
	}PLAYER_STATE_MOVE_DESC;
	enum PLAYER_MOVE_STATE { MOVE_IDLE, MOVE_WALK, MOVE_JOG, MOVE_DODGE, MOVE_END };
protected:
	CPlayer_Move();
	virtual ~CPlayer_Move() = default;

public:
	HRESULT Initialize(void* pArg) override;
	HRESULT StartState(void** pArg) override;
	void Priority_Update(_float fTimeDelta) override;
	void Update(_float fTimeDelta) override;
	void Late_Update(_float fTimeDelta) override;
	HRESULT Render() override;
	HRESULT ExitState(void** pArg) override;

private:
	void Move_Control(_float fTimeDelta);
	void Walk(_float fTimeDelta);
	void Jog(_float fTimeDelta);
	void Dodge();
	void Dodge_Control();


	void Change_State();

private:
	_float m_fDodgeDelay = { 0.f };
	PLAYER_MOVE_STATE m_eMoveState = { MOVE_END };

	_bool* m_pNonIntersect = { nullptr };

public:
	static CPlayer_Move* Create(void* pArg);
	virtual void Free();
};
END
