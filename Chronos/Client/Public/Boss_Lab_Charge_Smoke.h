#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect_Instance;
END

BEGIN(Client)
class CBoss_Lab_Charge_Smoke final : public CPartObject
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

		_float3 vPivot;
		_float3 vScale;
		_float fSpeed;
		_float fGravity;
		_float4 vColor;
		_float3 vPos;
	}BOSSCHARGE_SMOKE_DESC;
private:
	CBoss_Lab_Charge_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Lab_Charge_Smoke(const CBoss_Lab_Charge_Smoke& Prototype);
	virtual ~CBoss_Lab_Charge_Smoke() = default;

public:
	void Set_Loop(_bool bLoop) {
		m_bLoop = bLoop;
		if (true == m_bLoop)
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
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect_Instance* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };

private:
	_float m_fSpeed = { 0.f };
	_float m_fGravity = { 0.f };
	_float3 m_vScale = {};
	_float4 m_vColor = {};
	_float3 m_vPivot = {};

private:
	_bool m_bLoop = { false };
	_bool m_bOn = { false };

private:
	HRESULT Ready_Components(const BOSSCHARGE_SMOKE_DESC& Desc);

public:
	static CBoss_Lab_Charge_Smoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
