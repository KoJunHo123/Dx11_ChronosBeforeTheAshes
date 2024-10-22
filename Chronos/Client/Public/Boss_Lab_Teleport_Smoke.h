#pragma once

#include "PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect_Instance;
class CTransform;
END

BEGIN(Client)
class CBoss_Lab_Teleport_Smoke final : public CPartObject
{
public:
	typedef struct :CGameObject::GAMEOBJECT_DESC
	{
		class CTransform* pBossTransformCom;

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

		_bool isLoop;
	} BOSSTELEPORT_SMOKE_DESC;

private:
	CBoss_Lab_Teleport_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Lab_Teleport_Smoke(const CBoss_Lab_Teleport_Smoke& Prototype);
	virtual ~CBoss_Lab_Teleport_Smoke() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_On(_fvector vPos) {
		m_bOn = true;
		Set_Position(vPos);
	}

	void Set_Off();

	_bool Get_Dead() {
		return m_bDead;
	}

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect_Instance* m_pVIBufferCom = { nullptr };

private:
	_float m_fSpeed = { 0.f };
	_float m_fGravity = { 0.f };
	_float3 m_vScale = {};
	_float m_fRatioSpeed = { 0 };
	_float4 m_vColor = {};
	_float3 m_vPivot = {};

	_float m_fStartSpeed = { 0.f };

	_bool m_bOn = { false };
	_bool m_isLoop = { false };

private:
	HRESULT Ready_Components(const BOSSTELEPORT_SMOKE_DESC& Desc);

public:
	static CBoss_Lab_Teleport_Smoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};
END