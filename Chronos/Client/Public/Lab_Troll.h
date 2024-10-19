#pragma once

#include "Monster.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CLab_Troll final : public CMonster
{
public:
	enum PARTID { PART_BODY, PART_WEAPON_L, PART_WEAPON_R, PART_EFFECT_DEATH, PART_EFFECT_APPEAR, PART_END };
	enum STATE { STATE_ATTACK, STATE_SPRINT, STATE_IDLE, STATE_IMPACT, STATE_SPAWN, STATE_WALK, STATE_DEATH, STATE_END };

private:
	CLab_Troll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLab_Troll(const CLab_Troll& Prototype);
	virtual ~CLab_Troll() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval) override;

public:
	virtual void Be_Damaged(_float fDamage, _fvector vAttackPos);

private:
	class CCollider* m_pColliderCom = { nullptr };

private:
	_float m_fAttackDelay = { 0.f };
	_float m_fAttackTime = { 0.f };

	// ÆÄÃ÷¶û °øÀ¯ÇÏ´Â º¯¼ö
	_uint m_iState = { STATE_END };
	_bool m_isFinished = { false };
	_float m_fDistance = { 0.f };
	_bool m_bLeftAttackActive = { false };
	_bool m_bRightAttackActive = { false };

private:
	virtual HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

public:
	static CLab_Troll* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
