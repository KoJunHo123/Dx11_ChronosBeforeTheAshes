#pragma once

#include "Client_Defines.h"
#include "Blend_UI_Object.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_TextBox final : public CBlend_UI_Object
{
public:
	typedef struct : CBlend_UI_Object::UI_DESC
	{
		_wstring strText;
		_float fOffsetX;
	}UI_TEXTBOX_DESC;

private:
	CUI_TextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_TextBox(const CUI_TextBox& Prototype);
	virtual ~CUI_TextBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual _bool On_MousePoint(POINT ptMouse) override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_wstring m_strText = TEXT("");
	_float m_fOffsetX = { 0.f };

private:
	_float m_fRatio = { 0.f };
	_bool m_bAlphaControl = { false };
	POINT m_ptMouse = {};
	_bool m_bLevelChange = { false };

private:
	HRESULT Ready_Components();

public:
	static CUI_TextBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};

END