#pragma once

#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Client)
class CItem abstract : public CGameObject
{
public:
	enum TYPE { TYPE_DRAGONHEART, TYPE_DRAGONSTONE, TYPE_CONSUMABLE, TYPE_EQUIPMENT, TYPE_END };

public:
	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
	}ITEM_DESC;
	
protected:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CItem& Prototype);
	virtual ~CItem() = default;

public:
	virtual void Add_Item() {
		++m_iItemCount;
	}

	_uint Get_ItemCount() {
		return m_iItemCount;
	}


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(const _float4x4& WorldMatrix);

	virtual _bool Use_Item(class CPlayer* pPlayer = nullptr);

protected:
	_uint m_iItemCount = { 0 };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual CGameObject* Pooling() = 0;
	virtual void Free();
};
END
