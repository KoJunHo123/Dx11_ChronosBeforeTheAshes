#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CUIObject;
END

BEGIN(Client)
class CUI_PlayerBase final : public CGameObject
{
public:
	typedef struct : CGameObject::GAMEOBJECT_DESC
	{

	}PLAYERBASE_DESC;

	enum UI { UI_HP, UI_STAMINA, UI_SKILL, UI_DRAGONSTONE, UI_DRAGONHEART, UI_YOUDIED, UI_INTERACTION, UI_PUZZLE, UI_PUZZLEEXIT, UI_END };

private:
	CUI_PlayerBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerBase(const CUI_PlayerBase& Prototype);
	virtual ~CUI_PlayerBase() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CUIObject* m_UIObjects[UI_END] = {};
	class CPlayer* m_pPlayer = { nullptr };

private:
	HRESULT Ready_UI();

public:
	static CUI_PlayerBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END