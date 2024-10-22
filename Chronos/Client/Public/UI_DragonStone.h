#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CUI_DragonStone final : public CUIObject
{
public:
	typedef struct : CUIObject::UI_DESC
	{
		class CPlayer* pPlayer = nullptr;
	}UI_DRAGONSTONE_DESC;

private:
	CUI_DragonStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_DragonStone(const CUI_DragonStone& Prototype);
	virtual ~CUI_DragonStone() = default;

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
	class CTexture* m_pCoreTextureCom_Red = { nullptr };
	class CTexture* m_pCoreTextureCom_Pupple = { nullptr };

	class CPlayer* m_pPlayer = { nullptr };

private:
	_uint m_iCurrentSkill = { 0 };

private:
	HRESULT Ready_Components();

public:
	static CUI_DragonStone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;

};
END
