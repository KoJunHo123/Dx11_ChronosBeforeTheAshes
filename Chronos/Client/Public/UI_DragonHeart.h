#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CUI_DragonHeart final : public CUIObject
{
public:
	typedef struct : CUIObject::UI_DESC
	{
		class CDragonHeart* pDragonHeart = { nullptr };
	}UI_DRAGONHEART_DESC;

private:
	CUI_DragonHeart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_DragonHeart(const CUI_DragonHeart& Prototype);
	virtual ~CUI_DragonHeart() = default;

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
	class CDragonHeart* m_pDragonHeart = { nullptr };

private:
	_uint m_iItemCount = { 0 };
	_uint m_iMaxCount = { 0 };

private:
	HRESULT Ready_Components();

public:
	static CUI_DragonHeart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;

};
END
