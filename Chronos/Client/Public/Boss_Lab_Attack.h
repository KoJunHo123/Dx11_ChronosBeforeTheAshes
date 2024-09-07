#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CBoss_Lab_Attack final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pSocketMatrix;
		_float3 vExtents;
		_float3 vCenter;
		_float3 vAngles;
		_uint iDamage;
		_bool* pAttackActive;
	}ATTACK_DESC;

private:
	CBoss_Lab_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Lab_Attack(const CBoss_Lab_Attack& Prototype);
	virtual ~CBoss_Lab_Attack() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval) override;

private:
	class CCollider* m_pColliderCom = {nullptr};
private:
	const _float4x4* m_pSocketMatrix = {};

private:
	_uint m_iDamage = { 0 };
	_bool* m_pAttackActive = { nullptr };

private:
	HRESULT Ready_Components(_float3 vExtents, _float3 vCenter, _float3 vAngles);

public:
	static CBoss_Lab_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
