#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CUI_GageBar final : public CUIObject
{
public:
	enum TYPE { TYPE_HP, TYPE_STAMINA, TYPE_SKILL, TYPE_END };

	typedef struct : CUIObject::UI_DESC
	{
		class CPlayer* pPlayer = { nullptr };
		TYPE eType = { TYPE_END };
	}UI_GAGEBAR_DESC;

private:
	CUI_GageBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_GageBar(const CUI_GageBar& Prototype);
	virtual ~CUI_GageBar() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	class CTexture* m_pBorderTextureCom = { nullptr };
	class CTexture* m_pCoreTextureCom = { nullptr };

private:
	class CPlayer* m_pPlayer = { nullptr };
	TYPE m_eType = { TYPE_END };
	_float4 m_vColor = {};

private:
	_float m_fGage = { 0 };
	_float m_fMaxGage = { 0 };
	_float m_fRatio = { 0.f };

private:
	HRESULT Ready_Components();

public:
	static CUI_GageBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END
