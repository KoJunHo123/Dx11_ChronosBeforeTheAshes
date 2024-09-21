#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CParticle_Flash final : public CBlendObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos;
	}FLASH_DESC;
private:
	CParticle_Flash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Flash(const CParticle_Flash& Prototype);
	virtual ~CParticle_Flash() = default;

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
	_float3 m_vPivot = {};
	_float m_fSpeed = { 0.f };
	_bool m_bSpread = { false };

	_float4 m_vTeleportColor = {};

	_float2 m_vDivide = { };
	_float m_fTexIndex = { 0 };

	_bool m_bSmoke = { false };

private:
	HRESULT Ready_Components();

public:
	static CParticle_Flash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
