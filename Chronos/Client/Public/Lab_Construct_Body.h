#pragma once

#include "PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CNavigation;
class CModel;
class CShader;
END

BEGIN(Client)
class CLab_Construct_Body final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		class CTransform* pConstruct_TransformCom = { nullptr };
		class CNavigation* pNavigationCom = { nullptr };
		class CTexture* pNoiseTextureCom = { nullptr };

		_uint* pState = { nullptr };
		_bool* pIsFinished = { nullptr };
		_int* pHP = { nullptr };
		_float* pDistance = { nullptr };
		_bool* pSwordAttackActive = { nullptr };
		_bool* pShieldAttackActive = { nullptr };
		_float* pRatio = { nullptr };
	}BODY_DESC;

private:
	CLab_Construct_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLab_Construct_Body(const CLab_Construct_Body& Prototype);
	virtual ~CLab_Construct_Body() = default;

public:
	void Set_HittedAngle(_float fHittedAngle) {
		m_fHittedAngle = fHittedAngle;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Reset_Animation();
	_uint Get_FrameIndex();
	const _float4x4* Get_BoneMatrix_Ptr(const _char* pBoneName) const;


private:
	_bool Animation_Loop();
	_bool Animation_NonInterpolate();

private:
	class CTransform* m_pConstruct_TransformCom = { nullptr };
	class CNavigation* m_pNavigationCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CTexture* m_pNoiseTextureCom = { nullptr };

private:
	CONSTRUCT_ANIM m_eConstructAnim = { CONSTRUCT_ANIM_END };
	_float m_fHittedAngle = { 0.f };

	_bool m_bAnimStart = { false };

	_float m_fWalkDirChange = { 0.f };
	_float m_fSpeed = { 0.f };

private:
	_uint* m_pState = { nullptr };
	_bool* m_pIsFinished = { nullptr };
	_int* m_pHP = { nullptr };
	_float* m_pDistance = { nullptr };
	_bool* m_pSwordAttackActive = { nullptr };
	_bool* m_pShieldAttackActive = { nullptr };
	_float* m_pRatio = { nullptr };

private:
	HRESULT Ready_Components();
private:
	void Play_Animation(_float fTimeDelta);

public:
	static CLab_Construct_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END
