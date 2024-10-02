#pragma once

#include "Player_State.h"

BEGIN(Client)
class CPlayer_Impact final : public CPlayer_State
{
public:
	typedef struct : public CPlayer_State::PLAYER_STATE_DESC
	{
		_int* pHP = { nullptr };
	}PLAYER_STATE_IMPACT_DESC;

private:
	CPlayer_Impact();
	virtual ~CPlayer_Impact() = default;

public:
	void Set_HittedAngle(_float fHittedAngle) {
		m_fHittedAngle = fHittedAngle;
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
	_int* m_pHP = { nullptr };
	_bool* m_pDead = { nullptr };
	_float m_fHittedAngle = { 0.f };
	
public:
	static CPlayer_Impact* Create(void* pArg);
	virtual void Free();
};
END
