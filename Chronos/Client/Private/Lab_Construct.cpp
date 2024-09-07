#include "stdafx.h"
#include "Lab_Construct.h"
#include "GameInstance.h"

#include "Lab_Construct_Body.h"

CLab_Construct::CLab_Construct(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster(pDevice, pContext)
{
}

CLab_Construct::CLab_Construct(const CLab_Construct& Prototype)
    : CMonster(Prototype)
{
}

HRESULT CLab_Construct::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLab_Construct::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_eConstructAnim = LAB_CONSTRUCT_IDLE;

    _vector vPos = XMVectorSet(0.f, 0.f, 1.f, 0.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_pNavigationCom->Get_CellZXCenter(10)) + vPos);

    return S_OK;
}

_uint CLab_Construct::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    m_pColliderCom->Set_OnCollision(true);

    for (auto& Part : m_Parts)
    {
        if (nullptr == Part)
            continue;
        Part->Priority_Update(fTimeDelta);
    }

    return OBJ_NOEVENT;
}

void CLab_Construct::Update(_float fTimeDelta)
{
    for (auto& Part : m_Parts)
    {
        if (nullptr == Part)
            continue;
        Part->Update(fTimeDelta);
    }
    __super::Update(fTimeDelta);
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

}

void CLab_Construct::Late_Update(_float fTimeDelta)
{
    for (auto& Part : m_Parts)
    {
        if (nullptr == Part)
            continue;
        Part->Late_Update(fTimeDelta);
    }

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLab_Construct::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif

    return S_OK;
}

void CLab_Construct::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
    __super::Intersect(strColliderTag, pCollisionObject, vSourInterval, vDestInterval);

}

HRESULT CLab_Construct::Ready_Components()
{
    /* For.Com_Collider_AABB */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderAABBDesc{};
    ColliderAABBDesc.vExtents = _float3(3.f, 3.f, 3.f);
    ColliderAABBDesc.vCenter = _float3(0.f, ColliderAABBDesc.vExtents.y, 0.f);

    CCollider::COLLIDER_DESC ColliderDesc = {};
    ColliderDesc.pOwnerObject = this;
    ColliderDesc.pBoundingDesc = &ColliderAABBDesc;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;
    m_pGameInstance->Add_Collider_OnLayers(TEXT("Coll_Monster"), m_pColliderCom);


    return S_OK;
}

HRESULT CLab_Construct::Ready_PartObjects()
{
    m_Parts.resize(PART_END);

    CLab_Construct_Body::BODY_DESC BodyDesc = {};
    BodyDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.fSpeedPerSec = 1.f;
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    
    BodyDesc.pConstruct_TransformCom = m_pTransformCom;
    BodyDesc.pNavigationCom = m_pNavigationCom;
    
    BodyDesc.pConstructAnim = &m_eConstructAnim;
    BodyDesc.pIsFinished = &m_isFinished;

    if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Lab_Construct_Body"), &BodyDesc)))
        return E_FAIL;


    return S_OK;
}

CLab_Construct* CLab_Construct::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLab_Construct* pInstance = new CLab_Construct(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CLab_Construct"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLab_Construct::Clone(void* pArg)
{
    CLab_Construct* pInstance = new CLab_Construct(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CLab_Construct"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLab_Construct::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
}
