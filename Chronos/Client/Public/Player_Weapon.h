#pragma once

#include "Player_Part.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTransform;
class CCollider;
class CVIBuffer_Trail_TwoPoint_Instance;
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
		_bool* pDrain = { nullptr };
		_float* pHP = { nullptr };
		_float fMaxHP = { 0 };
		_float* pStamina = { nullptr };
		_float* pSkillGage = { nullptr };
		_float fMaxSkillGage = { 0.f };
		_float* pSkillDuration = { nullptr };
		_float fStartSpeed = { 0.f };

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
	virtual HRESULT Render_LightDepth() override;

public:
	virtual void Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval) override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

	class CCollider* m_pColliderCom = { nullptr };
	class CCollider* m_pSkillColliderCom = { nullptr };

	class CTexture* m_pNoiseTextureCom = { nullptr };


private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	const _float4x4* m_pTailSocketMatrix = { nullptr };

private:
	_float3 m_vTailPos = {};
	_float3 m_vPrePosition = {};
	_bool m_bStaminaDown = { false };
	_bool m_bFirst = { false };

private:
	_float m_fDamage = { 0.f };
	_bool m_bAttackActive = { false };
	_float* m_pRatio = { nullptr };
	_bool* m_pDrain = { nullptr };
	_float* m_pHP = { nullptr };
	_float m_fMaxHP = { 0 };
	_float* m_pStamina = { nullptr };
	_float* m_pSkillGage = { nullptr };
	_float m_fMaxSkillGage = { 0.f };
	_float* m_pSkillDuration = { nullptr };
	_float m_fStartSpeed = { 0.f };

private:
	HRESULT Ready_Components();
	HRESULT Add_AttackParticle(_fvector vPos, _fvector vPivot);

	_bool IsAttackAnim();
	void PlaySound();

public:
	static CPlayer_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();

};
END
