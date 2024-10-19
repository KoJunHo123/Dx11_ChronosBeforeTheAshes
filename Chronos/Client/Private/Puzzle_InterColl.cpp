#include "stdafx.h"
#include "Puzzle_InterColl.h"
#include "GameInstance.h"

#include "Camera_Container.h"
#include "Camera_Interaction.h"

#include "Inventory.h"
#include "Item.h"

CPuzzle_InterColl::CPuzzle_InterColl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CPuzzle_InterColl::CPuzzle_InterColl(const CPuzzle_InterColl& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CPuzzle_InterColl::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPuzzle_InterColl::Initialize(void* pArg)
{
	PART_INTERCOLL_DESC*  pDesc = static_cast<PART_INTERCOLL_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pColliderCom->Set_OnCollision(true);

	m_pInventory = static_cast<CInventory*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Inventory"), 0));
	m_pPuzzleReplace = pDesc->pPuzzleReplace;
	return S_OK;
}

_uint CPuzzle_InterColl::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CPuzzle_InterColl::Update(_float fTimeDelta)
{
	if (true == m_bPuzzleSolve)
	{
		if (m_pGameInstance->Get_DIKeyState_Down(DIK_TAB))
		{
			CItem* pItem = m_pInventory->Find_Item(TEXT("Item_ReplacePuzzle"));
			if (nullptr != pItem && true == pItem->Use_Item())
			{
				*m_pPuzzleReplace = true;

				SOUND_DESC desc = {};
				desc.fVolume = 1.f;
				m_pGameInstance->SoundPlay_Additional(TEXT("SFX_Puzzle_Toy_Platform_Raise_Lock_03.ogg"), desc);
			}
		}
	}

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CPuzzle_InterColl::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CPuzzle_InterColl::Render()
{
	return S_OK;
}

void CPuzzle_InterColl::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
	if (TEXT("Coll_Player") == strColliderTag)
	{
		CCamera_Container* pContainer = static_cast<CCamera_Container*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), 0));
		if(m_pGameInstance->Get_DIKeyState_Down(DIKEYBOARD_E))
		{
			if(false == m_bPuzzleSolve)
			{
				pContainer->Set_CurrentCamera(CCamera_Container::CAMERA_INTERACTION);
				pContainer->Set_InteractionTarget(XMLoadFloat4x4(&m_WorldMatrix).r[3], _float3(0.f, 5.f, -5.f));
				m_bPuzzleSolve = true;
			}
			else
			{
				pContainer->Set_CurrentCamera(CCamera_Container::CAMERA_SHORDER);
				m_bPuzzleSolve = false;
			}
		}
	}
}

HRESULT CPuzzle_InterColl::Ready_Components()
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

CPuzzle_InterColl* CPuzzle_InterColl::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPuzzle_InterColl* pInstance = new CPuzzle_InterColl(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CPuzzle_InterColl"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPuzzle_InterColl::Clone(void* pArg)
{
	CPuzzle_InterColl* pInstance = new CPuzzle_InterColl(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Create Failed : CPuzzle_InterColl"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPuzzle_InterColl::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
}
