#pragma once

#include "PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)
class CPlayer_Skill_Particle final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };
		const _float4x4* pTailSocketMatrix = { nullptr };

		_uint* pCurrentSkill = { nullptr };
		_float* pSkilDuration = { nullptr };
	}PLAYER_SKILL_DESC;

private:
	CPlayer_Skill_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Skill_Particle(const CPartObject& Prototype);
	virtual ~CPlayer_Skill_Particle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	class CTexture* m_pFireTextureCom = { nullptr };
	class CTexture* m_pShadowTextureCom = { nullptr };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	const _float4x4* m_pTailSocketMatrix = { nullptr };
	_uint* m_pCurrentSkill = { nullptr };
	_float* m_pSkilDuration = { nullptr };

private:
	_float2 m_vDivide = {};

	_bool m_bReset = { false };

private:
	HRESULT Ready_Components();

public:
	static CPlayer_Skill_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
