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

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->vExtents)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));

	return S_OK;
}

void CInteractionObject::Priority_Update(_float fTimeDelta)
{
	
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

HRESULT CInteractionObject::Ready_Components(_float3 vExtents)
{

	return S_OK;
}


void CInteractionObject::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
