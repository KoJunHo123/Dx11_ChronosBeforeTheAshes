#pragma once

#include "Player_State.h"

BEGIN(Client)
class CPlayer_Dodge final : public CPlayer_State
{
protected:
	CPlayer_Dodge();
	virtual ~CPlayer_Dodge() = default;

public:
	HRESULT Initialize(void* pArg) override;
	HRESULT StartState() override;
	void Priority_Update(_float fTimeDelta) override;
	void Update(_float fTimeDelta) override;
	void Late_Update(_float fTimeDelta) override;
	HRESULT Render() override;
	HRESULT ExitState() override;

public:
	static CPlayer_Dodge* Create(void* pArg);
	virtual void Free();
};
END
