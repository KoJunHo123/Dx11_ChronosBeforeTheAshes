#include "stdafx.h"
#include "Lab_Construct.h"
#include "GameInstance.h"

#include "Lab_Construct_Body.h"
#include "Lab_Construct_Attack.h"

#include "Particle_Monster_Death.h"
#include "Particle_Spawn.h"

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

    m_iMaxHP = 100;
    m_iHP = m_iMaxHP;

    m_iState = STATE_IDLE;
    m_pColliderCom->Set_OnCollision(true);

    return S_OK;
}

_uint CLab_Construct::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;


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
    m_fDistance = m_pTransformCom->Get_Distance(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

    if (m_fDistance < 30.f)
        m_bAggro = true;

    if(STATE_ATTACK != m_iState)
    {
        m_bSwordAttackActive = false;
        m_bShieldAttackActive = false;
    }

    if(true == m_bAggro)
    {
        if (STATE_IDLE == m_iState || STATE_WALK == m_iState)
            m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION), 0.1f);

        if(STATE_IDLE == m_iState || STATE_WALK == m_iState)
        {
            if ((m_fAttackTime < m_fAttackDelay) && m_fDistance < 15.f)
                m_iState = STATE_ATTACK;
            else
                m_iState = STATE_WALK;
        }
    }

    m_fAttackDelay += fTimeDelta;

    if (m_iHP <= 0 && STATE_DEATH != m_iState)
    {
        m_iState = STATE_DEATH;
        m_isFinished = false;
        m_pColliderCom->Set_OnCollision(false);
    }

    if (m_iState == STATE_DEATH && true == m_isFinished)
    {
        m_fRatio += fTimeDelta * 0.5f;
        static_cast<CParticle_Monster_Death*>(m_Parts[PART_EFFECT_DEATH])->Set_On();
    }

    if (true == m_isFinished && STATE_DEATH != m_iState)
    {
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

void CLab_Construct::Late_Update(_float fTimeDelta)
{
    for (auto& Part : m_Parts)
    {
        if (nullptr == Part)
            continue;
        Part->Late_Update(fTimeDelta);
    }

    if (1.f < m_fRatio && true == static_cast<CParticle_Monster_Death*>(m_Parts[PART_EFFECT_DEATH])->Get_Dead())
        m_bDead = true;

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
#ifdef _DEBUG
    m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CLab_Construct::Render()
{

    return S_OK;
}

void CLab_Construct::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
    __super::Intersect(strColliderTag, pCollisionObject, vSourInterval, vDestInterval);

}

void CLab_Construct::Be_Damaged(_uint iDamage, _fvector vAttackPos)
{
    m_iHP -= iDamage;

    if(iDamage > 15)
    {
        _vector vDir = vAttackPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

        vDir = XMVector3Normalize(vDir);
        vLook = XMVector3Normalize(vLook);

        _float fDot = XMVectorGetX(XMVector3Dot(vDir, vLook));

        _float fRadian = acos(fDot);

        if (0.f > XMVector3Cross(vDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK)).m128_f32[1])
            fRadian *= -1.f;

        CLab_Construct_Body* pBody = static_cast<CLab_Construct_Body*>(m_Parts[PART_BODY]);
        pBody->Set_HittedAngle(XMConvertToDegrees(fRadian));

        if (STATE_IMPACT == m_iState)
            pBody->Reset_Animation();
        else
            m_iState = STATE_IMPACT;
    }
}

HRESULT CLab_Construct::Ready_Components()
{
    /* For.Com_Collider_AABB */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderAABBDesc{};
    ColliderAABBDesc.vExtents = _float3(1.f, 3.f, 1.f);
    ColliderAABBDesc.vCenter = _float3(0.f, ColliderAABBDesc.vExtents.y, 0.f);

    CCollider::COLLIDER_DESC ColliderDesc = {};
    ColliderDesc.pOwnerObject = this;
    ColliderDesc.pBoundingDesc = &ColliderAABBDesc;
    ColliderDesc.strColliderTag = TEXT("Coll_Monster");

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

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
    
    BodyDesc.pState = &m_iState;
    BodyDesc.pIsFinished = &m_isFinished;
    BodyDesc.pHP = &m_iHP;
    BodyDesc.pDistance = &m_fDistance;
    BodyDesc.pSwordAttackActive = &m_bSwordAttackActive;
    BodyDesc.pShieldAttackActive = &m_bShieldAttackActive;
    BodyDesc.pNoiseTextureCom = m_pNoiseTextureCom;
    BodyDesc.pRatio = &m_fRatio;

    if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Lab_Construct_Body"), &BodyDesc)))
        return E_FAIL;


    CLab_Construct_Attack::ATTACK_DESC AttackDesc = {};
    AttackDesc.fRotationPerSec = 0.f;
    AttackDesc.fSpeedPerSec = 0.f;
    AttackDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    AttackDesc.pSocketMatrix = dynamic_cast<CLab_Construct_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LC_Weapon_Sword");
    AttackDesc.vAngles = { 0.f, 0.f, 0.f };
    AttackDesc.vCenter = { 0.f, -3.f, 0.f };
    AttackDesc.vExtents = { 1.f, 3.5f, 1.f };
    AttackDesc.pAttackActive = &m_bSwordAttackActive;
    AttackDesc.iDamage = 10;
    if (FAILED(__super::Add_PartObject(PART_SWORD, TEXT("Prototype_GameObject_Lab_Construct_Attack"), &AttackDesc)))
        return E_FAIL;

    AttackDesc.pSocketMatrix = dynamic_cast<CLab_Construct_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LC_Hand_L");
    AttackDesc.vAngles = { 0.f, 0.f, 0.f };
    AttackDesc.vCenter = { 0.f, 0.5f, 0.f };
    AttackDesc.vExtents = { 0.5f, 0.5f, 0.5f };
    AttackDesc.pAttackActive = &m_bShieldAttackActive;
    AttackDesc.iDamage = 5;
    if (FAILED(__super::Add_PartObject(PART_SHIELD, TEXT("Prototype_GameObject_Lab_Construct_Attack"), &AttackDesc)))
        return E_FAIL;

    CParticle_Monster_Death::PARTICLE_DEATH_DESC DeathDesc = {};
    DeathDesc.fRotationPerSec = 0.f;
    DeathDesc.fSpeedPerSec = 0.f;
    DeathDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    DeathDesc.pSocketMatrix = static_cast<CLab_Construct_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LC_Spine1");;
    DeathDesc.iNumInstance = 1500;
    DeathDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);
    DeathDesc.vRange = _float3(3.f, 3.f, 3.f);
    DeathDesc.vExceptRange = _float3(0.f, 0.f, 0.f);
    DeathDesc.vSize = _float2(0.15f, 0.3f);
    DeathDesc.vSpeed = _float2(1.5f, 3.f);
    DeathDesc.vLifeTime = _float2(1.f, 2.f);;

    if (FAILED(__super::Add_PartObject(PART_EFFECT_DEATH, TEXT("Prototype_GameObject_Particle_Monster_Death"), &DeathDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLab_Construct::Add_SpawnParticle(_fvector vPos, _float fOffset)
{
    CParticle_Spawn::PARTICLE_SPAWN_DESC desc = {};

    desc.fRotationPerSec = 0.f;
    desc.fSpeedPerSec = 1.f;
    XMStoreFloat3(&desc.vPos, vPos);
    desc.vPos.y += fOffset;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Spawn"), &desc)))
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
