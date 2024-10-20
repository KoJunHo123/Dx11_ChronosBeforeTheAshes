#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CEffect_Distortion final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos;
	} DISTORTION_DESC;

private:
	CEffect_Distortion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Distortion(const CEffect_Distortion& Prototype);
	virtual ~CEffect_Distortion() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	class CTexture* m_pNoiseTextureCom = { nullptr };
	class CTexture* m_pGlowTexxtureCom = { nullptr };

private:
	_float m_fTime = { 0.f };

private:
	HRESULT Ready_Components();

public:
	static CEffect_Distortion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
