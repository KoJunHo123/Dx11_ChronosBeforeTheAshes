#pragma once

#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect_Instance;
END

BEGIN(Client)
class CParticle_Smoke final : public CBlendObject
{
public:
	typedef struct :CGameObject::GAMEOBJECT_DESC
	{
		_uint iNumInstance;
		_float3 vCenter;
		_float3 vRange;
		_float3 vExceptRange;
		_float2 vLifeTime;
		_float4 vMaxColor;
		_float4 vMinColor;
		_float2 vSize;
		_float2 vSpeed;

		_float3 vPos;
		_float3 vScale;
		_float4 vColor;
	} SMOKE_DESC;

private:
	CParticle_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Smoke(const CParticle_Smoke& Prototype);
	virtual ~CParticle_Smoke() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect_Instance* m_pVIBufferCom = { nullptr };

private:
	_float m_fSpeed = { 0.f };
	_float3 m_vScale = {};
	_float m_fRatioSpeed = { 0 };
	_float4 m_vColor = {};
	
private:
	HRESULT Ready_Components(const SMOKE_DESC& Desc);

public:
	static CParticle_Smoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};
END
