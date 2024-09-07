#include "stdafx.h"
#include "Lab_Mage.h"
#include "GameInstance.h"

#include "Lab_Mage_Body.h"

CLab_Mage::CLab_Mage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster(pDevice, pContext)
{
}

CLab_Mage::CLab_Mage(const CLab_Mage& Prototype)
    : CMonster(Prototype)
{
}

HRESULT CLab_Mage::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLab_Mage::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_eMageAnim = LAB_MAGE_IDLE;
    _vector vPos = XMVectorSet(0.f, 0.f, 1.f, 0.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_pNavigationCom->Get_CellZXCenter(30)) + vPos);


    return S_OK;
}

_uint CLab_Mage::Priority_Update(_float fTimeDelta)
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

void CLab_Mage::Update(_float fTimeDelta)
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

void CLab_Mage::Late_Update(_float fTimeDelta)
{
    for (auto& Part : m_Parts)
    {
        if (nullptr == Part)
            continue;
        Part->Late_Update(fTimeDelta);
    }

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLab_Mage::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif

    return S_OK;
}

void CLab_Mage::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
}

HRESULT CLab_Mage::Ready_Components()
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

HRESULT CLab_Mage::Ready_PartObjects()
{
    m_Parts.resize(PART_END);

    CLab_Mage_Body::BODY_DESC BodyDesc = {};
    BodyDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.fSpeedPerSec = 1.f;
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    BodyDesc.pConstruct_TransformCom = m_pTransformCom;
    BodyDesc.pNavigationCom = m_pNavigationCom;

    BodyDesc.pMageAnim = &m_eMageAnim;
    BodyDesc.pIsFinished = &m_isFinished;

    if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Lab_Mage_Body"), &BodyDesc)))
        return E_FAIL;

    return S_OK;
}

CLab_Mage* CLab_Mage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLab_Mage* pInstance = new CLab_Mage(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CLab_Mage"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLab_Mage::Clone(void* pArg)
{
    CLab_Mage* pInstance = new CLab_Mage(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CLab_Mage"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLab_Mage::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);

}
