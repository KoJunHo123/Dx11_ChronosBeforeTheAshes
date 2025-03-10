#pragma once

#include "Client_Defines.h"
#include "Blend_UI_Object.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_GlowStar final : public CBlend_UI_Object
{
private:
	CUI_GlowStar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_GlowStar(const CUI_GlowStar& Prototype);
	virtual ~CUI_GlowStar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = {nullptr};
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float m_fRatio = { 0.f };
	_bool m_bAlphaControl = { false };

private:
	HRESULT Ready_Components();

public:
	static CUI_GlowStar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;

};

END