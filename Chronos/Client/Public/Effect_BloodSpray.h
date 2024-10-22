#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CEffect_BloodSpray final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos;
		_float3 vScale;
		_float3 vDir;
		_float fIndexSpeed;
	}BLOOD_DESC;

private:
	CEffect_BloodSpray(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_BloodSpray(const CEffect_BloodSpray& Prototype);
	virtual ~CEffect_BloodSpray() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CTexture* m_pDiffTextureCom = { nullptr };
	class CTexture* m_pNormTextureCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_uint m_iCoreNum = { 0 };

	_float2 m_vDivide = { };
	_float m_fTexIndex = { 0.f };
	_float m_fIndexSpeed = { 0.f };
	_float3 m_vMoveDir = { };

	_float m_fSpeed = { 0.f };
private:
	HRESULT Ready_Components();

public:
	static CEffect_BloodSpray* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};
END
