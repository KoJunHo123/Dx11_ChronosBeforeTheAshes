#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CEffect_Flash final : public CBlendObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos;
		_float4 vColor;
	}FLASH_DESC;
private:
	CEffect_Flash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Flash(const CEffect_Flash& Prototype);
	virtual ~CEffect_Flash() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CTexture* m_pTeleport_TextureCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
private:

	_float4 m_vTeleportColor = {};

	_float2 m_vDivide = { };
	_float m_fTexIndex = { 0 };


private:
	HRESULT Ready_Components();

public:
	static CEffect_Flash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
