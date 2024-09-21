#pragma once

#include "Player_Part.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTransform;
class CCollider;
END

BEGIN(Client)
class CPlayer_Weapon final : public CPlayer_Part
{
public:
	typedef struct : public CPlayer_Part::PLAYER_PART_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };
		const _float4x4* pTailSocketMatrix = { nullptr };
		class CTexture* pNoiseTextureCom = { nullptr };

		_float* pRatio = { nullptr };
	}PLAYER_WEAPON_DESC;
private:
	CPlayer_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Weapon(const CPlayer_Weapon& Prototype);
	virtual ~CPlayer_Weapon() = default;

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
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CCollider* m_pColliderCom = { nullptr };
	class CTexture* m_pNoiseTextureCom = { nullptr };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	const _float4x4* m_pTailSocketMatrix = { nullptr };

private:
	_float3 m_vTailPos = {};

private:
	_uint m_iDamage = { 0 };
	_bool m_bAttackActive = { false };
	_float* m_pRatio = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Add_AttackParticle(_fvector vPos, _fvector vPivot);

public:
	static CPlayer_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();

};
END
