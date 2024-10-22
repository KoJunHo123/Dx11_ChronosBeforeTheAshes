#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CUI_Interaction final : public CUIObject
{
public:
	typedef struct : CUIObject::UI_DESC
	{
		class CPlayer* pPlayer = { nullptr };
	}UI_INTERACTION_DESC;

private:
	CUI_Interaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Interaction(const CUI_Interaction& Prototype);
	virtual ~CUI_Interaction() = default;


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
	class CTexture* m_pTextureCom = { nullptr };

private:
	class CPlayer* m_pPlayer = { nullptr };
	_float m_fZ = { 0.f };

	_bool m_bOn = { false };

private:
	HRESULT Ready_Components();

public:
	static CUI_Interaction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;

};
END
