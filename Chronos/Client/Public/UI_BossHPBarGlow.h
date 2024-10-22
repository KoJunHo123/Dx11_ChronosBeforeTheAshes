#pragma once

#include "Client_Defines.h"
#include "Blend_UI_Object.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CUI_BossHPBarGlow final : public CBlend_UI_Object
{
public:
	typedef struct : CBlend_UI_Object::UI_DESC
	{
		class CMonster* pMonster = { nullptr };
	}UI_HPBARGLOW_DESC;

private:
	CUI_BossHPBarGlow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_BossHPBarGlow(const CUI_BossHPBarGlow& Prototype);
	virtual ~CUI_BossHPBarGlow() = default;


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
	static CUI_BossHPBarGlow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};
END
