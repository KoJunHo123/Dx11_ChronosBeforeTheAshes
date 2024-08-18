#pragma once

#include "Player_State.h"

BEGIN(Client)
class CPlayer_Block final : public CPlayer_State
{
	enum PLAYER_BLOCK_STATE{ BLOCK_START, BLOCK_IDLE, BLOCK_WALK, BLOCK_IMPACT, BLOCK_IMPACT_BREAK, BLOCK_END };

protected:
	CPlayer_Block();
	virtual ~CPlayer_Block() = default;

public:
	HRESULT Initialize(void* pArg) override;
	HRESULT StartState(void** pArg) override;
	void Priority_Update(_float fTimeDelta) override;
	void Update(_float fTimeDelta) override;
	void Late_Update(_float fTimeDelta) override;
	HRESULT Render() override;
	HRESULT ExitState(void** pArg) override;

private:
	void Walk(_float fTimeDelta);

private:
	PLAYER_BLOCK_STATE m_eBlockState = { BLOCK_END };

public:
	static CPlayer_Block* Create(void* pArg);
	virtual void Free();
};
END
