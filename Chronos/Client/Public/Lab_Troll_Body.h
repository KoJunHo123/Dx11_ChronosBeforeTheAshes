#pragma once

#include "PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CNavigation;
class CModel;
class CShader;
class CTexture;
END

BEGIN(Client)
class CLab_Troll_Body final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		class CTransform* pConstruct_TransformCom = { nullptr };
		class CNavigation* pNavigationCom = { nullptr };
		class CTexture* pNoiseTextureCom = { nullptr };

		_uint* pState = { nullptr };
		_bool* pIsFinished = { nullptr };
		_float* pHP = { nullptr };
		_float* pDistance = { nullptr };
		_bool* pRightAttackActive = { nullptr };
		_bool* pLeftAttackActive = { nullptr };
		_float* pRatio = { nullptr };
	}BODY_DESC;

private:
	CLab_Troll_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLab_Troll_Body(const CLab_Troll_Body& Prototype);
	virtual ~CLab_Troll_Body() = default;

public:
	void Set_HittedDesc(_float fHittedAngle, _int iImpactedDamage) {
		m_fHittedAngle = fHittedAngle;
		m_iImpactedDamage = iImpactedDamage;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

public:
	void Reset_Animation();
	const _float4x4* Get_BoneMatrix_Ptr(const _char* pBoneName) const;
	_uint Get_FrameIndex();

private:
	_bool Animation_Loop();
	_bool Animation_NonInterpolate();
	void StepSound();

private:
	class CTransform* m_pTroll_TransformCom = { nullptr };
	class CNavigation* m_pNavigationCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CTexture* m_pNoiseTextureCom = { nullptr };

private:
	TROLL_ANIM m_eTrollAnim = { TROLL_ANIM_END };
	_float m_fHittedAngle = { 0.f };
	_int m_iImpactedDamage = { 0 };
	_bool m_bAnimStart = { false };
	_float m_fSpeed = { 0.f };
	_float m_fMoveDirDelay = { 0.f };

	_bool m_bLeftStep = { false };
	_bool m_bRightStep = { false };

	_bool m_bLeftSprint = { false };
	_bool m_bRightSprint = { false };


	SOUND_DESC m_SoundDesc = {};

private:
	_uint* m_pState = { nullptr };
	_bool* m_pIsFinished = { nullptr };
	_float* m_pHP = { nullptr };
	_float* m_pDistance = { nullptr };
	_bool* m_pLeftAttackActive = { nullptr };
	_bool* m_pRightAttackActive = { nullptr };
	_float* m_pRatio = { nullptr };

private:
	HRESULT Ready_Components();

private:
	void Play_Animation(_float fTimeDelta);
	void Play_AttackSound();

public:
	static CLab_Troll_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};
END
