#pragma once

#include "Blend_PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CPlayer_Skill final : public CBlend_PartObject
{
public:
	typedef struct : public CBlend_PartObject::PARTOBJ_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };
		const _float4x4* pTailSocketMatrix = { nullptr };

		_uint* pCurrentSkill = { nullptr };
		_float* pSkilDuration = { nullptr };
	}PLAYER_SKILL_DESC;

private:
	CPlayer_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Skill(const CPlayer_Skill& Prototype);
	virtual ~CPlayer_Skill() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CTexture* m_pFireTextureCom = { nullptr };
	class CTexture* m_pShadowTextureCom = { nullptr };

	class CTexture* m_pLightTextureCom = { nullptr };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	const _float4x4* m_pTailSocketMatrix = { nullptr };
	_uint* m_pCurrentSkill = { nullptr };
	_float* m_pSkilDuration = { nullptr };

private:
	_float3 m_vTailPos = {};
	_float3 m_vPrePosition = {};

	_float2 m_vDivide = { };
	_float m_fTexIndex = { 0 };


private:
	HRESULT Ready_Components();

public:
	static CPlayer_Skill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free();
};
END
