#pragma once

#include "Player_Part.h"

BEGIN(Engine)
class CTransform;
END


// 여기서는 애니메이션 돌릴 때 아이템 위치용임. 실제 사용은 밖에서.
BEGIN(Client)
class CPlayer_Item final : public CPlayer_Part
{
public:
	typedef struct : public CPlayer_Part::PLAYER_PART_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };
		class CInventory* pInventory = { nullptr };
	}PLAYER_ITEM_DESC;

private:
	CPlayer_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Item(const CPlayer_Item& Prototype);
	virtual ~CPlayer_Item() = default;


public:
	_bool IsEmpty() {
		if (nullptr == m_pItem)
			return true;
		return false;
	}

	void Set_Item(const _wstring strItemKey);

	void Release_Item() {
		Safe_Release(m_pItem);
		m_pItem = nullptr;
	}

	_vector Get_Position() {
		return XMLoadFloat4x4(&m_WorldMatrix).r[3];
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	class CInventory* m_pInventory = { nullptr };
	class CItem* m_pItem = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CPlayer_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();

};
END
