#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CUI_BossHPBarDeco final : public CUIObject
{
public:
	typedef struct : CUIObject::UI_DESC
	{
		class CMonster* pMonster = { nullptr };
	}UI_HPBARDECO_DESC;

private:
	CUI_BossHPBarDeco(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BossHPBarDeco(const CUI_BossHPBarDeco& Prototype);
	virtual ~CUI_BossHPBarDeco() = default;


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
	class CMonster* m_pMonster = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CUI_BossHPBarDeco* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
