#include "stdafx.h"
#include "Lab_Troll.h"
#include "GameInstance.h"

#include "Lab_Troll_Body.h"
#include "Lab_Troll_Weapon.h"

#include "Particle_Monster_Death.h"
#include "Particle_Monster_Appear.h"

CLab_Troll::CLab_Troll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster(pDevice, pContext)
{
}

CLab_Troll::CLab_Troll(const CLab_Troll& Prototype)
    : CMonster(Prototype)
{
}

HRESULT CLab_Troll::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLab_Troll::Initialize(void* pArg)
{
    m_eMonsterType = MONSTER_TROLL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_fMaxHP = 30.f;
    m_fHP = m_fMaxHP;

    m_iState = STATE_SPAWN;
    m_pColliderCom->Set_OnCollision(true);

    m_fOffset = 4.f;

    m_fRatio = 1.f;
    m_bStart = true;

    return S_OK;
}

_uint CLab_Troll::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

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

void CLab_Troll::Update(_float fTimeDelta)
{
    m_fDistance = m_pTransformCom->Get_Distance(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));


    if (m_fDistance < 30.f && false == m_bAggro)
    {
        m_bAggro = true;
        Add_MonsterHPBar();
    }

    if (STATE_ATTACK != m_iState)
    {
        m_bLeftAttackActive = false;
        m_bRightAttackActive = false;
    }

    _bool bLook = { false };
    if (true == m_bAggro)
    {
        if (STATE_IDLE == m_iState || STATE_WALK == m_iState || STATE_SPRINT == m_iState)
        {
            _vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
            vPlayerPos.m128_f32[1] = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];
            bLook = m_pTransformCom->LookAt(vPlayerPos, 0.1f);
        }

        if (STATE_IDLE == m_iState || STATE_WALK == m_iState || STATE_SPRINT == m_iState)
        {
            if ((m_fAttackTime < m_fAttackDelay) && m_fDistance < 10.f)
            {
                    m_iState = STATE_ATTACK;
            }
            else
            {
                if (m_fDistance > 10.f && true == bLook)
                    m_iState = STATE_SPRINT;
                else
                    m_iState = STATE_WALK;
            }
        }
    }

    m_fAttackDelay += fTimeDelta;

    if(STATE_DEATH == m_iState)
        m_fRatio += fTimeDelta * 0.5f;

    if (m_fHP <= 0.f)
    {
        m_iState = STATE_DEATH;
        m_isFinished = false;
        static_cast<CParticle_Monster_Death*>(m_Parts[PART_EFFECT_DEATH])->Set_On();
        m_pColliderCom->Set_OnCollision(false);

        if(false == m_bScream)
        {
            SOUND_DESC desc = {};
            desc.fMaxDistance = DEFAULT_DISTANCE;
            desc.fVolume = 1.f;
            XMStoreFloat3(&desc.vPos, Get_Position());

            m_pGameInstance->SoundPlay_Additional(TEXT("Troll_VO_Death_03.ogg"), desc);

            m_bScream = true;
        }

    }

    if (true == m_isFinished && STATE_DEATH != m_iState)
    {
        m_iState = STATE_IDLE;
        
        if(STATE_IMPACT != m_iState)
        {
            m_fAttackTime = m_pGameInstance->Get_Random(1.f, 3.f);
            m_fAttackDelay = 0.f;
        }
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

void CLab_Troll::Late_Update(_float fTimeDelta)
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

HRESULT CLab_Troll::Render()
{

    return S_OK;
}

void CLab_Troll::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
    __super::Intersect(strColliderTag, pCollisionObject, vSourInterval, vDestInterval);

}

void CLab_Troll::Be_Damaged(_float fDamage, _fvector vAttackPos)
{
    __super::Be_Damaged(fDamage, vAttackPos);

    m_fHP -= fDamage;
    _vector vDir = vAttackPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

    vDir = XMVector3Normalize(vDir);
    vLook = XMVector3Normalize(vLook);

    _float fDot = XMVectorGetX(XMVector3Dot(vDir, vLook));

    _float fRadian = acos(fDot);

    if (0.f > XMVector3Cross(vDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK)).m128_f32[1])
        fRadian *= -1.f;

    CLab_Troll_Body* pBody = static_cast<CLab_Troll_Body*>(m_Parts[PART_BODY]);
    pBody->Set_HittedDesc(XMConvertToDegrees(fRadian), fDamage);

    if (STATE_IMPACT == m_iState)
        pBody->Reset_Animation();
    else
        m_iState = STATE_IMPACT;

    SOUND_DESC desc = {};
    desc.fMaxDistance = DEFAULT_DISTANCE;
    desc.fVolume = 1.f;
    XMStoreFloat3(&desc.vPos, Get_Position());

    m_pGameInstance->SoundPlay_Additional(TEXT("Troll_VO_Pain_Short_03.ogg"), desc);
    cout << "왜 안나와" << endl;
}

HRESULT CLab_Troll::Ready_Components()
{
    /* For.Com_Collider_AABB */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderAABBDesc{};
    ColliderAABBDesc.vExtents = _float3(1.f, 2.f, 1.f);
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

HRESULT CLab_Troll::Ready_PartObjects()
{
    m_Parts.resize(PART_END);

    CLab_Troll_Body::BODY_DESC BodyDesc = {};
    BodyDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.fSpeedPerSec = 1.f;
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    BodyDesc.pConstruct_TransformCom = m_pTransformCom;
    BodyDesc.pNavigationCom = m_pNavigationCom;

    BodyDesc.pState = &m_iState;
    BodyDesc.pIsFinished = &m_isFinished;
    BodyDesc.pHP = &m_fHP;
    BodyDesc.pDistance = &m_fDistance;
    BodyDesc.pLeftAttackActive = &m_bLeftAttackActive;
    BodyDesc.pRightAttackActive = &m_bRightAttackActive;
    BodyDesc.pRatio = &m_fRatio;
    BodyDesc.pNoiseTextureCom = m_pNoiseTextureCom;

    if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Lab_Troll_Body"), &BodyDesc)))
        return E_FAIL;

    CLab_Troll_Weapon::WEAPON_DESC WeaponDesc = {};
    WeaponDesc.fRotationPerSec = XMConvertToRadians(90.f);
    WeaponDesc.fSpeedPerSec = 1.f;
    WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    WeaponDesc.pSocketBoneMatrix = static_cast<CLab_Troll_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LT_Weapon_Dagger_L");;
    WeaponDesc.vAngles = { 0.f, 0.f, 0.f };
    WeaponDesc.vCenter = { 0.f, 0.f, 0.5f };
    WeaponDesc.vExtents = { 1.f, 1.f, 1.f };
    WeaponDesc.pAttackActive = &m_bLeftAttackActive;
    WeaponDesc.fDamage = 10.f;
    WeaponDesc.pRatio = &m_fRatio;
    WeaponDesc.pNoiseTextureCom = m_pNoiseTextureCom;

    if (FAILED(__super::Add_PartObject(PART_WEAPON_L, TEXT("Prototype_GameObject_Lab_Troll_Weapon"), &WeaponDesc)))
        return E_FAIL;
  
    WeaponDesc.pSocketBoneMatrix = static_cast<CLab_Troll_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LT_Weapon_Dagger_R");
    WeaponDesc.pAttackActive = &m_bRightAttackActive;   
    WeaponDesc.fDamage = 10.f;

    if (FAILED(__super::Add_PartObject(PART_WEAPON_R, TEXT("Prototype_GameObject_Lab_Troll_Weapon"), &WeaponDesc)))
        return E_FAIL;

    CParticle_Monster_Death::PARTICLE_DEATH_DESC DeathDesc = {};
    DeathDesc.fRotationPerSec = 0.f;
    DeathDesc.fSpeedPerSec = 0.f;
    DeathDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    DeathDesc.pSocketMatrix = static_cast<CLab_Troll_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LT_Spine1");
    DeathDesc.iNumInstance = 100;
    DeathDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);
    DeathDesc.vRange = _float3(1.f, 1.f, 1.f);
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
    AppearDesc.pSocketMatrix = static_cast<CLab_Troll_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LT_Spine1");
    AppearDesc.iNumInstance = 300;
    AppearDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);
    AppearDesc.vRange = _float3(6.f, 6.f, 6.f);
    AppearDesc.vExceptRange = _float3(0.f, 0.f, 0.f);
    AppearDesc.vSize = _float2(0.15f, 0.3f);
    AppearDesc.vSpeed = _float2(2.f, 4.f);
    AppearDesc.vLifeTime = _float2(1.f, 2.f);;

    if (FAILED(__super::Add_PartObject(PART_EFFECT_APPEAR, TEXT("Prototype_GameObject_Particle_Monster_Appear"), &AppearDesc)))
        return E_FAIL;

    return S_OK;
}


CLab_Troll* CLab_Troll::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLab_Troll* pInstance = new CLab_Troll(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CLab_Troll"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLab_Troll::Clone(void* pArg)
{
    CLab_Troll* pInstance = new CLab_Troll(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CLab_Troll"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLab_Troll::Pooling()
{
    return new CLab_Troll(*this);
}

void CLab_Troll::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
}
