#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect_Instance;
END

BEGIN(Client)
class CDoorLock_Effect final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
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

		_float3 vMoveDir;
		_float3 vScale;
		_float fSpeed;
		_float fGravity;
		_float4 vColor;

		_bool* pLoop;
	}EFFECT_DESC;

private:
	CDoorLock_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDoorLock_Effect(const CDoorLock_Effect& Prototype);
	virtual ~CDoorLock_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect_Instance* m_pVIBufferCom = { nullptr };

	_float m_fSpeed = { 0.f };
	_float m_fGravity = { 0.f };
	_float3 m_vScale = {};
	_float4 m_vColor = {};
	_float3 m_vMoveDir = {};

private:
	_bool* m_pLoop = { nullptr };

private:
	HRESULT Ready_Components(const EFFECT_DESC& Desc);

public:
	static CDoorLock_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};
END