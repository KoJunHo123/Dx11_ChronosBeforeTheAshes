#include "stdafx.h"
#include "Teleport.h"
#include "GameInstance.h"

CTeleport::CTeleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CTeleport::CTeleport(const CTeleport& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CTeleport::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTeleport::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    TELEPORT_DESC* pDesc = static_cast<TELEPORT_DESC*>(pArg);   
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPos));
    m_pColliderCom->Set_OnCollision(true);

    m_vTeleportPos = pDesc->vTeleportPos;
	return S_OK;
}

_uint CTeleport::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CTeleport::Update(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CTeleport::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTeleport::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

#ifdef _DEBUG
    m_pColliderCom->Render();
#endif

    return S_OK;
}

void CTeleport::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
    if (TEXT("Coll_Player") == strColliderTag && m_pGameInstance->Get_DIKeyState_Down(DIKEYBOARD_E))
    {
        
        if (XMVector3Equal(XMLoadFloat3(&m_vTeleportPos), XMVectorSet(0.f, 0.f, 0.f, 0.f)))
            return;

        pCollisionObject->Set_Position(XMLoadFloat3(&m_vTeleportPos));
    }
}

HRESULT CTeleport::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Teleport"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider_AABB */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderAABBDesc{};
    ColliderAABBDesc.vExtents = _float3(1.f, 2.f, 1.f);
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

CTeleport* CTeleport::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTeleport* pInstance = new CTeleport(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CTeleport"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CTeleport::Clone(void* pArg)
{
    CTeleport* pInstance = new CTeleport(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Create Failed : CTeleport"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CTeleport::Free()
{
    __super::Free();
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
