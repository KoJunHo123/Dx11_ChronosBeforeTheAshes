#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CBoss_Lab_Body final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		class CTransform* pBossTransformCom = { nullptr };
		class CNavigation* pNavigationCom = { nullptr };
		class CTexture* pNoiseTextureCom = { nullptr };

		_uint* pState = { nullptr };
		_bool* pAnimOver = { nullptr };
		_bool* pAnimStart = { nullptr };
		_bool* pAttackActive_LH = { nullptr };
		_bool* pAttackActive_RH = { nullptr };
		_bool* pAttackActive_Body = { nullptr };
		_float* pRatio = { nullptr };
	}BODY_DESC;

private:
	CBoss_Lab_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Lab_Body(const CBoss_Lab_Body& Prototype);
	virtual ~CBoss_Lab_Body() = default;

public:
	const _float4x4* Get_BoneMatrix_Ptr(const _char* pBoneName) const;

	BOSS_ANIM Get_CurrentAnim() {
		return m_eBossAnim;
	}

	void Set_Intro(_bool bIntro) {
		m_bIntro = bIntro;
	}
	_uint Get_FrameIndex();


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

private:
	class CTransform* m_pBossTransformCom = { nullptr };
	class CNavigation* m_pNavigationCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CTexture* m_pNoiseTextureCom = { nullptr };

private:
	BOSS_ANIM			m_eBossAnim = { BOSS_ANIM_END };
	_bool				m_isFinished = { false };
	_bool				m_bIntro = { false };

	_bool				m_bDig = { false };
	_bool				m_bLaunch = { false };


private:
	_float				m_fChargingTime = { 0.f };
	_float				m_fChargeSpeed = { 0.f };

	_float				m_fStunTime = { 0.f };

	_float				m_fTeleportTime = { 0.f };
	_uint				m_iTeleportCount = { 0 };

	SOUND_DESC			m_SoundDesc = {};

private:
	_uint* m_pState = { nullptr };
	_bool* m_pAnimStart = { nullptr };
	_bool* m_pAnimOver = { nullptr };

	_bool* m_pAttackActive_LH = { nullptr };
	_bool* m_pAttackActive_RH = { nullptr };
	_bool* m_pAttackActive_Body = { nullptr };
	_float* m_pRatio = { nullptr };

private:
	HRESULT Ready_Components();

private:
	void Play_Animation(_float fTimeDelta);
	void Play_Sound_Short();
	

public:
	static CBoss_Lab_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
