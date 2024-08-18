#pragma once

#include "Player_State.h"

BEGIN(Client)
class CPlayer_Impact final : public CPlayer_State
{
protected:
	CPlayer_Impact();
	virtual ~CPlayer_Impact() = default;

public:
	HRESULT Initialize(void* pArg) override;
	HRESULT StartState(void** pArg) override;
	void Priority_Update(_float fTimeDelta) override;
	void Update(_float fTimeDelta) override;
	void Late_Update(_float fTimeDelta) override;
	HRESULT Render() override;
	HRESULT ExitState(void** pArg) override;

public:
	static CPlayer_Impact* Create(void* pArg);
	virtual void Free();
};
END
