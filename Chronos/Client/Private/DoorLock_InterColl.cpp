#include "stdafx.h"
#include "DoorLock_InterColl.h"
#include "GameInstance.h"

#include "Inventory.h"
#include "Item.h"

#include "Player.h"

CDoorLock_InterColl::CDoorLock_InterColl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CDoorLock_InterColl::CDoorLock_InterColl(const CDoorLock_InterColl& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CDoorLock_InterColl::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDoorLock_InterColl::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	INTERCOLL_DESC* desc = static_cast<INTERCOLL_DESC*>(pArg);
	
	if (FAILED(Ready_Components(desc->fOffset)))
		return E_FAIL;

	m_pColliderCom->Set_OnCollision(true);
	m_pUnLock = desc->pUnLock;

	return S_OK;
}

_uint CDoorLock_InterColl::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CDoorLock_InterColl::Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CDoorLock_InterColl::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CDoorLock_InterColl::Render()
{
	return S_OK;
}

void CDoorLock_InterColl::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
	if (TEXT("Coll_Player") == strColliderTag)
	{
		if (m_pGameInstance->Get_DIKeyState_Down(DIKEYBOARD_E))
		{
			_vector vPos = XMLoadFloat4x4(&m_WorldMatrix).r[3];
			vPos.m128_f32[0] -= 0.125f;
			vPos = XMVectorSetZ(vPos, XMVectorGetZ(vPos) - 4.f);
			*m_pUnLock = static_cast<CPlayer*>(pCollisionObject)->Use_Runekey(vPos, XMLoadFloat4x4(&m_WorldMatrix).r[3]);
		}
	}

}

HRESULT CDoorLock_InterColl::Ready_Components(_float fOffset)
{
	CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc = {};
	BoundingDesc.vCenter = { 0.f, fOffset, 0.f };
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

CDoorLock_InterColl* CDoorLock_InterColl::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDoorLock_InterColl* pInstance = new CDoorLock_InterColl(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CDoorLock_InterColl"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDoorLock_InterColl::Clone(void* pArg)
{
	CDoorLock_InterColl* pInstance = new CDoorLock_InterColl(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Create Failed : CDoorLock_InterColl"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDoorLock_InterColl::Pooling()
{
	return new CDoorLock_InterColl(*this);
}

void CDoorLock_InterColl::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
}
