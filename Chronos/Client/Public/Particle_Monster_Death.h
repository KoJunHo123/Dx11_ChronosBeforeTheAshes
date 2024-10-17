#pragma once

#include "PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect_Instance;
END

BEGIN(Client)
class CParticle_Monster_Death final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pSocketMatrix;

		_uint		iNumInstance;
		_float3		vCenter;
		_float3		vRange;
		_float3		vExceptRange;
		_float2		vSize;
		_float2		vSpeed;
		_float2		vLifeTime;
	} PARTICLE_DEATH_DESC;
private:
	CParticle_Monster_Death(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Monster_Death(const CParticle_Monster_Death& Prototype);
	virtual ~CParticle_Monster_Death() = default;

public:
	void Set_On() {
		m_bOn = true;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CTexture* m_pTextureCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect_Instance* m_pVIBufferCom = { nullptr };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	_bool m_bOn = { false };

private:
	_float3 m_vPivot = {};

private:
	HRESULT Ready_Components(PARTICLE_DEATH_DESC* pDesc);

public:
	static CParticle_Monster_Death* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END