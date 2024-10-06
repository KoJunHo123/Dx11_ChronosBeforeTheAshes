#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CMoedl;
class CShader;
class CCollider;
class CTexture;
END


BEGIN(Client)
class CLab_Troll_Weapon final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		class CTexture* pNoiseTextureCom = { nullptr };

		const _float4x4* pSocketBoneMatrix = { nullptr };
		_float3 vExtents;
		_float3 vCenter;
		_float3 vAngles;
		_float fDamage;
		_bool* pAttackActive;
		_float* pRatio;
	}WEAPON_DESC;

private:
	CLab_Troll_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLab_Troll_Weapon(const CLab_Troll_Weapon& Prototype);
	virtual ~CLab_Troll_Weapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval) override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CCollider* m_pColliderCom = { nullptr };
	class CTexture* m_pNoiseTextureCom = { nullptr };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };

private:
	_float m_fDamage = { 0 };
	_bool* m_pAttackActive = { nullptr };
	_float* m_pRatio = { nullptr };
	_float3 m_vCenter = {};

	_float3 m_vPrePosition = {};

private:
	HRESULT Ready_Components(_float3 vExtents, _float3 vCenter, _float3 vAngles);
	HRESULT Add_AttackParticle(_fvector vPos, _fvector vDir);

public:
	static CLab_Troll_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CLab_Troll_Weapon* Clone(void* pArg) override;
	virtual void Free() override;
};
END
