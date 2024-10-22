#pragma once

#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CEffect_Flare final : public CBlendObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos;
		_float3 vScale;
		_float4 vColor;
	}FLARE_DESC;
private:
	CEffect_Flare(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Flare(const CEffect_Flare& Prototype);
	virtual ~CEffect_Flare() = default;

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
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float4 m_vColor = {};
	_float3 m_vScale = {};
	_float3 m_vStartScale = {};

private:
	HRESULT Ready_Components();

public:
	static CEffect_Flare* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;

};
END
