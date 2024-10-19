#include "stdafx.h"
#include "Lab_Construct.h"
#include "GameInstance.h"

#include "Lab_Construct_Body.h"
#include "Lab_Construct_Attack.h"
#include "Lab_Construct_Effect_Black.h"
#include "Lab_Construct_Effect_Pupple.h"

#include "Particle_Monster_Death.h"
#include "Particle_Monster_Appear.h"

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
    m_eMonsterType = MONSTER_CONSTRUCT;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_fMaxHP = 100.f;
    m_fHP = m_fMaxHP;

    m_iState = STATE_IDLE;
    m_pColliderCom->Set_OnCollision(true);
    m_fOffset = 9.f;
    m_fRatio = 1.f;
    m_bStart = true;

    return S_OK;
}

_uint CLab_Construct::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    if (true == m_bSwordAttackActive)
    {
        static_cast<CLab_Construct_Attack*>(m_Parts[PART_SWORD])->Set_Anim(static_cast<CLab_Construct_Body*>(m_Parts[PART_BODY])->Get_Anim());
    }
    else if (true == m_bShieldAttackActive)
    {
        static_cast<CLab_Construct_Attack*>(m_Parts[PART_SHIELD])->Set_Anim(static_cast<CLab_Construct_Body*>(m_Parts[PART_BODY])->Get_Anim());
    }

    if (true == m_bStart)
    {
        m_fRatio -= fTimeDelta * 0.5f;
        if (m_fRatio < 0.f)
        {
            m_fRatio = 0.f;
            m_bStart = false;
        }
    }

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

    if (m_fDistance < 30.f && false == m_bAggro)
    {
        m_bAggro = true;
        Add_MonsterHPBar();
    }

    if(STATE_ATTACK != m_iState)
    {
        m_bSwordAttackActive = false;
        m_bShieldAttackActive = false;
    }

    if(true == m_bAggro)
    {
        if (STATE_IDLE == m_iState || STATE_WALK == m_iState)
        {
            _vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
            vPlayerPos.m128_f32[1] = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];
            m_pTransformCom->LookAt(vPlayerPos, 0.1f);
        }

        if(STATE_IDLE == m_iState || STATE_WALK == m_iState)
        {
            if ((m_fAttackTime < m_fAttackDelay) && m_fDistance < 15.f)
                m_iState = STATE_ATTACK;
            else
                m_iState = STATE_WALK;
        }
    }

    m_fAttackDelay += fTimeDelta;

    if (m_fHP <= 0.f && STATE_DEATH != m_iState)
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

    if (LAB_CONSTRUCT_ATK_3HIT_COMBO == m_iCurrentAnim)
    {
        _uint iCurrentFrame = static_cast<CLab_Construct_Body*>(m_Parts[PART_BODY])->Get_FrameIndex();
        if (30 < iCurrentFrame)
        {
            static_cast<CLab_Construct_Effect_Black*>(m_Parts[PART_EFFECT_BLACK])->Set_On(true);
            static_cast<CLab_Construct_Effect_Pupple*>(m_Parts[PART_EFFECT_PUPPLE])->Set_On(true);
        }
    }
    else
    {
        static_cast<CLab_Construct_Effect_Black*>(m_Parts[PART_EFFECT_BLACK])->Set_On(false);
        static_cast<CLab_Construct_Effect_Pupple*>(m_Parts[PART_EFFECT_PUPPLE])->Set_On(false);
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

void CLab_Construct::Be_Damaged(_float fDamage, _fvector vAttackPos)
{
    __super::Be_Damaged(fDamage, vAttackPos);
    m_fHP -= fDamage;

    if(fDamage > 15)
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
    BodyDesc.pHP = &m_fHP;
    BodyDesc.pDistance = &m_fDistance;
    BodyDesc.pSwordAttackActive = &m_bSwordAttackActive;
    BodyDesc.pShieldAttackActive = &m_bShieldAttackActive;
    BodyDesc.pNoiseTextureCom = m_pNoiseTextureCom;
    BodyDesc.pRatio = &m_fRatio;
    BodyDesc.pCurrentAnim = &m_iCurrentAnim;

    if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Lab_Construct_Body"), &BodyDesc)))
        return E_FAIL;


    CLab_Construct_Attack::ATTACK_DESC AttackDesc = {};
    AttackDesc.fRotationPerSec = 0.f;
    AttackDesc.fSpeedPerSec = 0.f;
    AttackDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    AttackDesc.pSocketMatrix = dynamic_cast<CLab_Construct_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LC_Weapon_Sword");
    AttackDesc.vAngles = { 0.f, 0.f, 0.f };
    AttackDesc.vCenter = { 0.f, -3.f, 0.f };
    AttackDesc.vExtents = { 1.f, 3.5f, 2.f };
    AttackDesc.pAttackActive = &m_bSwordAttackActive;
    AttackDesc.fDamage = 10.f;
    if (FAILED(__super::Add_PartObject(PART_SWORD, TEXT("Prototype_GameObject_Lab_Construct_Attack"), &AttackDesc)))
        return E_FAIL;

    AttackDesc.pSocketMatrix = dynamic_cast<CLab_Construct_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LC_Hand_L");
    AttackDesc.vAngles = { 0.f, 0.f, 0.f };
    AttackDesc.vCenter = { 0.f, 0.5f, 0.f };
    AttackDesc.vExtents = { 0.5f, 0.5f, 0.5f };
    AttackDesc.pAttackActive = &m_bShieldAttackActive;
    AttackDesc.fDamage = 5.f;
    if (FAILED(__super::Add_PartObject(PART_SHIELD, TEXT("Prototype_GameObject_Lab_Construct_Attack"), &AttackDesc)))
        return E_FAIL;

    CParticle_Monster_Death::PARTICLE_DEATH_DESC DeathDesc = {};
    DeathDesc.fRotationPerSec = 0.f;
    DeathDesc.fSpeedPerSec = 0.f;
    DeathDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    DeathDesc.pSocketMatrix = static_cast<CLab_Construct_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LC_Spine1");;
    DeathDesc.iNumInstance = 800;
    DeathDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);
    DeathDesc.vRange = _float3(3.f, 3.f, 3.f);
    DeathDesc.vExceptRange = _float3(0.f, 0.f, 0.f);
    DeathDesc.vSize = _float2(0.15f, 0.3f);
    DeathDesc.vSpeed = _float2(1.5f, 3.f);
    DeathDesc.vLifeTime = _float2(1.f, 2.f);;

    if (FAILED(__super::Add_PartObject(PART_EFFECT_DEATH, TEXT("Prototype_GameObject_Particle_Monster_Death"), &DeathDesc)))
        return E_FAIL;

    CParticle_Monster_Appear::PARTICLE_APPEAR_DESC AppearDesc = {};
    AppearDesc.fRotationPerSec = 0.f;
    AppearDesc.fSpeedPerSec = 0.f;
    AppearDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    AppearDesc.pSocketMatrix = static_cast<CLab_Construct_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LC_Spine1");
    AppearDesc.iNumInstance = 1200;
    AppearDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);
    AppearDesc.vRange = _float3(10.f, 10.f, 10.f);
    AppearDesc.vExceptRange = _float3(0.f, 0.f, 0.f);
    AppearDesc.vSize = _float2(0.15f, 0.3f);
    AppearDesc.vSpeed = _float2(2.f, 4.f);
    AppearDesc.vLifeTime = _float2(1.f, 2.f);;

    if (FAILED(__super::Add_PartObject(PART_EFFECT_APPEAR, TEXT("Prototype_GameObject_Particle_Monster_Appear"), &AppearDesc)))
        return E_FAIL;

    CLab_Construct_Effect_Black::EFFECT_DESC BlackDesc = {};
    BlackDesc.fRotationPerSec = 0.f;
    BlackDesc.fSpeedPerSec = 0.f;
    BlackDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BlackDesc.pSocketMatrix = dynamic_cast<CLab_Construct_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LC_Weapon_Sword");
   
    if (FAILED(__super::Add_PartObject(PART_EFFECT_BLACK, TEXT("Prototype_GameObject_Lab_Construct_Effect_Black"), &BlackDesc)))
        return E_FAIL;

    CLab_Construct_Effect_Pupple::EFFECT_DESC PuppleDesc = {};
    PuppleDesc.fRotationPerSec = 0.f;
    PuppleDesc.fSpeedPerSec = 0.f;
    PuppleDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    PuppleDesc.pSocketMatrix = dynamic_cast<CLab_Construct_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LC_Weapon_Sword");

    if (FAILED(__super::Add_PartObject(PART_EFFECT_PUPPLE, TEXT("Prototype_GameObject_Lab_Construct_Effect_Pupple"), &PuppleDesc)))
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
