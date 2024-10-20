#include "stdafx.h"
#include "Lab_Drum.h"
#include "GameInstance.h"

#include "Lab_Drum_Body.h"
#include "Lab_Drum_Attack.h"

#include "Particle_Monster_Death.h"
#include "Particle_Monster_Appear.h"

#include "Particle_Spawn.h"
#include "Effect_Spark.h"

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
    m_eMonsterType = MONSTER_DRUM;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_fMaxHP = 70.f;
    m_fHP = m_fMaxHP;

    m_iState = STATE_SPAWN;
    m_fSummonDelay = 45.f;
    m_pColliderCom->Set_OnCollision(true);

    m_fOffset = 6.f;
    m_fRatio = 1.f;
    m_bStart = true;


    return S_OK;
}

_uint CLab_Drum::Priority_Update(_float fTimeDelta)
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

void CLab_Drum::Update(_float fTimeDelta)
{
    m_fDistance = m_pTransformCom->Get_Distance(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

    if (m_fDistance < 30.f && false == m_bAggro)
    {
        m_bAggro = true;
        Add_MonsterHPBar();
    }
    if(STATE_ATTACK != m_iState)
    {
        m_bBellyAttackActive = false;
        m_bMaceAttackActive = false;
    }

    if (true == m_bAggro)
    {
        if (STATE_IDLE == m_iState || STATE_WALK == m_iState)
        {
            _vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
            vPlayerPos.m128_f32[1] = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];
            m_pTransformCom->LookAt(vPlayerPos, 0.1f);
        }

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

    if (STATE_SUMMON == m_iState)
    {
        if (false == m_bSummon && 45 == dynamic_cast<CLab_Drum_Body*>(m_Parts[PART_BODY])->Get_FrameIndex())
        {
            Add_SpawnParticle(1.f);
            m_bSummon = true;
        }
    }
    else
        m_bSummon = false;


    if (m_fHP <= 0.f && STATE_DEATH != m_iState)
    {
        m_iState = STATE_DEATH;
        m_isFinished = false;
        m_pColliderCom->Set_OnCollision(false);
        if (false == m_bScream)
        {
            SOUND_DESC SoundDesc = {};
            SoundDesc.fMaxDistance = DEFAULT_DISTANCE;
            SoundDesc.fVolume = 1.f;
            XMStoreFloat3(&SoundDesc.vPos, Get_Position());

            m_pGameInstance->SoundPlay_Additional(TEXT("Drummer_VO_Death_02.ogg"), SoundDesc);
            m_bScream = true;
        }
    }

    if (m_iState == STATE_DEATH && true == m_isFinished)
    {
        m_fRatio += fTimeDelta * 0.5f;
        static_cast<CParticle_Monster_Death*>(m_Parts[PART_EFFECT_DEATH])->Set_On();
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

void CLab_Drum::Late_Update(_float fTimeDelta)
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

HRESULT CLab_Drum::Render()
{

    return S_OK;
}

void CLab_Drum::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
    __super::Intersect(strColliderTag, pCollisionObject, vSourInterval, vDestInterval);

}

void CLab_Drum::Be_Damaged(_float fDamage, _fvector vAttackPos)
{
    __super::Be_Damaged(fDamage, vAttackPos);
    m_fHP -= fDamage;

    if (fDamage > 15.f)
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

        SOUND_DESC SoundDesc = {};
        SoundDesc.fMaxDistance = DEFAULT_DISTANCE;
        SoundDesc.fVolume = 1.f;
        XMStoreFloat3(&SoundDesc.vPos, Get_Position());

        m_pGameInstance->SoundPlay_Additional(TEXT("Drummer_VO_Pain_09.ogg"), SoundDesc);

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
    ColliderDesc.strColliderTag = TEXT("Coll_Monster");

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

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
    BodyDesc.pHP = &m_fHP;
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
    AttackDesc.fDamage = 10.f;

    if (FAILED(__super::Add_PartObject(PART_BELLY, TEXT("Prototype_GameObject_Lab_Drum_Attack"), &AttackDesc)))
        return E_FAIL;

    AttackDesc.pSocketMatrix = dynamic_cast<CLab_Drum_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LD_Tail_Weapon_6");
    AttackDesc.vAngles = { 0.f, 0.f, 0.f };
    AttackDesc.vCenter = { 0.f, 0.f, 0.f };
    AttackDesc.vExtents = { .5f, .5f, .5f };
    AttackDesc.pAttackActive = &m_bMaceAttackActive;
    AttackDesc.fDamage = 10.f;

    if (FAILED(__super::Add_PartObject(PART_MACE, TEXT("Prototype_GameObject_Lab_Drum_Attack"), &AttackDesc)))
        return E_FAIL;

    CParticle_Monster_Death::PARTICLE_DEATH_DESC DeathDesc = {};
    DeathDesc.fRotationPerSec = 0.f;
    DeathDesc.fSpeedPerSec = 0.f;
    DeathDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    DeathDesc.pSocketMatrix = static_cast<CLab_Drum_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LD_Spine1");;
    DeathDesc.iNumInstance = 400;
    DeathDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);
    DeathDesc.vRange = _float3(2.f, 2.f, 2.f);
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
    AppearDesc.pSocketMatrix = static_cast<CLab_Drum_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LD_Spine1");
    AppearDesc.iNumInstance = 600;
    AppearDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);
    AppearDesc.vRange = _float3(8.f, 8.f, 8.f);
    AppearDesc.vExceptRange = _float3(0.f, 0.f, 0.f);
    AppearDesc.vSize = _float2(0.15f, 0.3f);
    AppearDesc.vSpeed = _float2(2.f, 4.f);
    AppearDesc.vLifeTime = _float2(1.f, 2.f);;

    if (FAILED(__super::Add_PartObject(PART_EFFECT_APPEAR, TEXT("Prototype_GameObject_Particle_Monster_Appear"), &AppearDesc)))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CLab_Drum::Add_SpawnParticle(_float fOffset)
{
    CParticle_Spawn::PARTICLE_SPAWN_DESC desc = {};

    desc.fRotationPerSec = 0.f;
    desc.fSpeedPerSec = 1.f;
    desc.iSpawnCellIndex = m_pNavigationCom->Get_CanMoveCellIndex_InNear();
    XMStoreFloat3(&desc.vPos, m_pNavigationCom->Get_CellCenterPos(desc.iSpawnCellIndex));
    desc.vPos.y += fOffset;

    if (-1 == desc.iSpawnCellIndex)
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Spawn"), &desc)))
        return E_FAIL;

    CEffect_Spark::SPARK_DESC SparkDesc = {};
    SparkDesc.fRotationPerSec = XMConvertToRadians(90.f);
    SparkDesc.fSpeedPerSec = 0.f;
    SparkDesc.vPos = desc.vPos;
    //SparkDesc.vColor = { 0.188f, 0.098f, 0.204f, 1.f };
    SparkDesc.vColor = _float4(0.541f, 0.169f, 0.886f, 0.f);
    SparkDesc.vScale = { 8.f, 8.f, 8.f };
    SparkDesc.fIndexSpeed = 60.f;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Spark"), &SparkDesc)))
        return E_FAIL;

    SOUND_DESC SoundDesc = {};
    SoundDesc.fMaxDistance = DEFAULT_DISTANCE * 2.f;
    SoundDesc.fVolume = 1.f;
    XMStoreFloat3(&SoundDesc.vPos, Get_Position());

    m_pGameInstance->SoundPlay_Additional(TEXT("SFX_Drummer_Attack_HitDrum_Sweetener_Stereo_01.ogg"), SoundDesc);

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
