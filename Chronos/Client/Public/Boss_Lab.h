#pragma once

#include "Monster.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CBoss_Lab final : public CMonster
{
public:
	enum PARTID { PART_BODY, PART_ATTACK_LH, PART_ATTACK_RH, PART_ATTACK_BODY, PART_EFFECT_DEATH, 
		PART_TELEPORT_SMOKE, PART_TELEPORT_SMOKE_BOIL, PART_TELEPORT_SMOKE_SPLASH, PART_TELEPORT_STONE, PART_TELEPORT_LIGHT, 
		PART_CHARGE_SMOKE_LEFT, PART_CHARGE_SMOKE_RIGHT, PART_END };
	enum STATE { 
		STATE_CHARGE	= 0x0001, 
		STATE_RUSH		= 0x0002,
		STATE_NEAR		= 0x0004,
		STATE_IDLE		= 0x0008,
		STATE_WALK		= 0x0010,
		STATE_DEATH		= 0x0020,
		STATE_STUN		= 0x0040,
		STATE_TELEPORT	= 0x0080,
		STATE_IMPACT	= 0x0100,
		STATE_APPEAR	= 0x0200,
		STATE_END		= 0xffff 
	};

private:
	CBoss_Lab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Lab(const CBoss_Lab& Prototype);
	virtual ~CBoss_Lab() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval) override;
	virtual void Be_Damaged(_float fDamage, _fvector vAttackPos) override;

private:
	class CCollider* m_pColliderCom = { nullptr };

private:
	_float m_fAttackDelay = { 0.f };

private:
	// ÆÄÃ÷¶û °øÀ¯ÇÏ´Â º¯¼ö
	_uint m_iState = { STATE_END };
	_bool m_bAnimStart = { false };
	_bool m_bAnimOver = { false };
	_bool m_bFirstStun = { false };

	_bool m_bAttackActive_LH = { false };
	_bool m_bAttackActive_RH = { false };
	_bool m_bAttackActive_Body = { false };

	_bool m_bLaunchStart = { false };
	_bool m_bLaunchEffect = { false };
	_bool m_bLaunchOver = { false };
	_bool m_bEncounter = { false };

	_float m_fNextAttack = { 0.f };

private:
	virtual HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

private:
	_bool Contain_State(_uint iState);
	HRESULT Add_TeleportEffect();
	HRESULT Add_LaunchEffect(_fvector vPos);
	HRESULT Add_BossHPBar();

public:
	static CBoss_Lab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
