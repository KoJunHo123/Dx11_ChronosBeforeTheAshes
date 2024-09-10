#include "stdafx.h"
#include "InteractionObject.h"
#include "GameInstance.h"

CInteractionObject::CInteractionObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CInteractionObject::CInteractionObject(const CInteractionObject& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CInteractionObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteractionObject::Initialize(void* pArg)
{
	INTERACTION_OBJECT_DESC* pDesc = static_cast<INTERACTION_OBJECT_DESC*>(pArg);

	if(FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->vExtents)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));

	return S_OK;
}

_uint CInteractionObject::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CInteractionObject::Update(_float fTimeDelta)
{
}

void CInteractionObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CInteractionObject::Render()
{
	return S_OK;
}

void CInteractionObject::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{

}

HRESULT CInteractionObject::Ready_Components(_float3 vExtents)
{
	/* For.Com_Collider_AABB */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderAABBDesc{};
	ColliderAABBDesc.vExtents = vExtents;
	ColliderAABBDesc.vCenter = _float3(0.f, ColliderAABBDesc.vExtents.y, 0.f);

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.pOwnerObject = this;
	ColliderDesc.pBoundingDesc = &ColliderAABBDesc;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	m_pGameInstance->Add_Collider_OnLayers(TEXT("Coll_Interaction"), m_pColliderCom);

	return S_OK;
}


void CInteractionObject::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
