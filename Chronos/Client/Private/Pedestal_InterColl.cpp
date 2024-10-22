#include "stdafx.h"
#include "Pedestal_InterColl.h"
#include "GameInstance.h"

#include "Inventory.h"
#include "Item.h"

CPedestal_InterColl::CPedestal_InterColl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CPedestal_InterColl::CPedestal_InterColl(const CPedestal_InterColl& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CPedestal_InterColl::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPedestal_InterColl::Initialize(void* pArg)
{
	PART_INTERCOLL_DESC* pDesc = 	static_cast<PART_INTERCOLL_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pColliderCom->Set_OnCollision(true);
	m_ppItem = pDesc->ppItem;
	m_strItemTag = pDesc->strItemTag;

	return S_OK;
}

_uint CPedestal_InterColl::Priority_Update(_float fTimeDelta)
{
	if (nullptr == *m_ppItem)
		m_pColliderCom->Set_OnCollision(false);
	else
		m_pColliderCom->Set_OnCollision(true);

	return OBJ_NOEVENT;
}

void CPedestal_InterColl::Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CPedestal_InterColl::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CPedestal_InterColl::Render()
{
	return S_OK;
}

void CPedestal_InterColl::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
	if (TEXT("Coll_Player") == strColliderTag)
	{
		if (m_pGameInstance->Get_DIKeyState_Down(DIKEYBOARD_E))
		{
			CInventory* pInventory = static_cast<CInventory*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Inventory"), 0));

			pInventory->Add_Item(m_strItemTag, *m_ppItem);

			SOUND_DESC desc = {};
			desc.fVolume = 1.f;
			m_pGameInstance->SoundPlay_Additional(TEXT("Item_Pickup_Key_02.ogg"), desc);

			*m_ppItem = nullptr;
		}
	}
}

HRESULT CPedestal_InterColl::Ready_Components()
{
	CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc = {};
	BoundingDesc.vCenter = { 0.f, 0.f, 0.f };
	BoundingDesc.vExtents = { 5.f, 5.f, 5.f };

	CCollider::COLLIDER_DESC CollDesc = {};
	CollDesc.bCollisionOnce = false;
	CollDesc.pOwnerObject = this;
	CollDesc.strColliderTag = TEXT("Coll_Interaction");
	CollDesc.pBoundingDesc = &BoundingDesc;

	/* FOR.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollDesc)))
		return E_FAIL;

	return S_OK;
}

CPedestal_InterColl* CPedestal_InterColl::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPedestal_InterColl* pInstance = new CPedestal_InterColl(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CPedestal_InterColl"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPedestal_InterColl::Clone(void* pArg)
{
	CPedestal_InterColl* pInstance = new CPedestal_InterColl(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Create Failed : CPedestal_InterColl"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPedestal_InterColl::Pooling()
{
	return new CPedestal_InterColl(*this);
}

void CPedestal_InterColl::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
}
