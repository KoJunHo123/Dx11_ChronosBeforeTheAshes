#pragma once

#include "Monster.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CLab_Construct final : public CMonster
{
public:
	enum PARTID { PART_BODY, PART_EFFECT, PART_END };
	enum STATE { STATE_ATTACK, STATE_IDLE, STATE_JUMP, STATE_MOVE, STATE_END };

private:
	CLab_Construct(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLab_Construct(const CLab_Construct& Prototype);
	virtual ~CLab_Construct() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval) override;

private:
	class CCollider* m_pColliderCom = { nullptr };

	_float m_fSpeed = { 10.f };
	// ÆÄÃ÷¶û °øÀ¯ÇÏ´Â º¯¼ö
	CONSTRUCT_ANIM m_eConstructAnim = { CONSTRUCT_ANIM_END };
	_bool m_isFinished = { false };

private:
	virtual HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

public:
	static CLab_Construct* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
