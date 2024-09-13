#include "stdafx.h"
#include "Lab_Drum.h"
#include "GameInstance.h"

#include "Lab_Drum_Body.h"
#include "Lab_Drum_Attack.h"

CLab_Drum::CLab_Drum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster(pDevice, pContext)
{
}

CLab_Drum::CLab_Drum(const CLab_Drum& Prototype)
    : CMonster(Prototype)
{
}

HRESULT CLab_Drum::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLab_Drum::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_iMaxHP = 100;
    m_iHP = m_iMaxHP;

    m_iState = STATE_SPAWN;
    m_fSummonDelay = 45.f;

    return S_OK;
}

_uint CLab_Drum::Priority_Update(_float fTimeDelta)
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

void CLab_Drum::Update(_float fTimeDelta)
{
    m_fDistance = m_pTransformCom->Get_Distance(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

    if (m_fDistance < 30.f)
        m_bAggro = true;

    if(STATE_ATTACK != m_iState)
    {
        m_bBellyAttackActive = false;
        m_bMaceAttackActive = false;
    }

    if (true == m_bAggro)
    {
        if (STATE_IDLE == m_iState || STATE_WALK == m_iState)
            m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION), 0.1f);

        if (STATE_IDLE == m_iState || STATE_WALK == m_iState)
        {
            if ((m_fAttackTime < m_fAttackDelay) && m_fDistance < 6.f)
                m_iState = STATE_ATTACK;
            else if (45.f < m_fSummonDelay)
            {
                m_iState = STATE_SUMMON;
                m_fSummonDelay = 0.f;
            }
            else
                m_iState = STATE_WALK;
        }
        m_fSummonDelay += fTimeDelta;
    }

    m_fAttackDelay += fTimeDelta;

    if (m_iHP <= 0)
    {
        m_iState = STATE_DEATH;
        m_isFinished = false;
    }

    if (true == m_isFinished)
    {
        if(STATE_SUMMON == m_iState)
            Summon_Troll();

        m_iState = STATE_IDLE;
        m_fAttackTime = m_pGameInstance->Get_Random(1.f, 3.f);
        m_fAttackDelay = 0.f;
    }

    for (auto& Part : m_Parts)
    {
        if (nullptr == Part)
            continue;
        Part->Update(fTimeDelta);
    }
    __super::Update(fTimeDelta);
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

}

void CLab_Drum::Late_Update(_float fTimeDelta)
{
    for (auto& Part : m_Parts)
    {
        if (nullptr == Part)
            continue;
        Part->Late_Update(fTimeDelta);
    }

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLab_Drum::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif

    return S_OK;
}

void CLab_Drum::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
    __super::Intersect(strColliderTag, pCollisionObject, vSourInterval, vDestInterval);

}

void CLab_Drum::Be_Damaged(_uint iDamage, _fvector vAttackPos)
{
    m_iHP -= iDamage;

    if (iDamage > 15)
    {
        _vector vDir = vAttackPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

        vDir = XMVector3Normalize(vDir);
        vLook = XMVector3Normalize(vLook);

        _float fDot = XMVectorGetX(XMVector3Dot(vDir, vLook));

        _float fRadian = acos(fDot);

        if (0.f > XMVector3Cross(vDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK)).m128_f32[1])
            fRadian *= -1.f;

        CLab_Drum_Body* pBody = static_cast<CLab_Drum_Body*>(m_Parts[PART_BODY]);
        pBody->Set_HittedAngle(XMConvertToDegrees(fRadian));

        if (STATE_IMPACT == m_iState)
            pBody->Reset_Animation();
        else
            m_iState = STATE_IMPACT;
    }
}

HRESULT CLab_Drum::Ready_Components()
{
    /* For.Com_Collider_AABB */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderAABBDesc{};
    ColliderAABBDesc.vExtents = _float3(1.5f, 2.f, 1.5f);
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

HRESULT CLab_Drum::Ready_PartObjects()
{
    m_Parts.resize(PART_END);

    CLab_Drum_Body::BODY_DESC BodyDesc = {};
    BodyDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.fSpeedPerSec = 1.f;
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    BodyDesc.pConstruct_TransformCom = m_pTransformCom;
    BodyDesc.pNavigationCom = m_pNavigationCom;

    BodyDesc.pState = &m_iState;
    BodyDesc.pIsFinished = &m_isFinished;
    BodyDesc.pHP = &m_iHP;
    BodyDesc.pDistance = &m_fDistance;
    BodyDesc.pBellyAttackActive = &m_bBellyAttackActive;
    BodyDesc.pMaceAttackActive = &m_bMaceAttackActive;
    BodyDesc.pNoiseTextureCom = m_pNoiseTextureCom;
    BodyDesc.pRatio = &m_fRatio;

    if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Lab_Drum_Body"), &BodyDesc)))
        return E_FAIL;

    CLab_Drum_Attack::ATTACK_DESC AttackDesc = {};
    AttackDesc.fRotationPerSec = 0.f;
    AttackDesc.fSpeedPerSec = 0.f;
    AttackDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    AttackDesc.pSocketMatrix = dynamic_cast<CLab_Drum_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LD_Drum_Front");
    AttackDesc.vAngles = { 0.f, 0.f, 0.f };
    AttackDesc.vCenter = { 0.f, -1.f, 0.f };
    AttackDesc.vExtents = { 1.f, 1.f, 1.f };
    AttackDesc.pAttackActive = &m_bBellyAttackActive;
    AttackDesc.iDamage = 10;

    if (FAILED(__super::Add_PartObject(PART_BELLY, TEXT("Prototype_GameObject_Lab_Drum_Attack"), &AttackDesc)))
        return E_FAIL;

    AttackDesc.pSocketMatrix = dynamic_cast<CLab_Drum_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LD_Tail_Weapon_6");
    AttackDesc.vAngles = { 0.f, 0.f, 0.f };
    AttackDesc.vCenter = { 0.f, 0.f, 0.f };
    AttackDesc.vExtents = { .5f, .5f, .5f };
    AttackDesc.pAttackActive = &m_bMaceAttackActive;
    AttackDesc.iDamage = 10;

    if (FAILED(__super::Add_PartObject(PART_MACE, TEXT("Prototype_GameObject_Lab_Drum_Attack"), &AttackDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLab_Drum::Summon_Troll()
{
    CMonster::MONSTER_DESC desc = {};
    desc.fRotationPerSec = XMConvertToRadians(90.f);
    desc.fSpeedPerSec = 1.f;
    desc.vRotation = {};
    desc.vScale = { 1.f, 1.f, 1.f };
    desc.iStartCellIndex = m_pNavigationCom->Get_CanMoveCellIndex_InNear();

    if (-1 == desc.iStartCellIndex)
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Troll"), &desc)))
        return E_FAIL;

    return S_OK;
}

CLab_Drum* CLab_Drum::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLab_Drum* pInstance = new CLab_Drum(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CLab_Drum"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLab_Drum::Clone(void* pArg)
{
    CLab_Drum* pInstance = new CLab_Drum(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CLab_Drum"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLab_Drum::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
}
