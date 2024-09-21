#include "stdafx.h"
#include "Lab_Mage.h"
#include "GameInstance.h"

#include "Lab_Mage_Body.h"
#include "Lab_Mage_Attack.h"

#include "Particle_Monster_Death.h"
#include "Particle_Spawn.h"

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

    m_iMaxHP = 100;
    m_iHP = m_iMaxHP;

    m_iState = STATE_SPAWN;
    m_pColliderCom->Set_OnCollision(true);

    return S_OK;
}

_uint CLab_Mage::Priority_Update(_float fTimeDelta)
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

void CLab_Mage::Update(_float fTimeDelta)
{
    m_fDistance = m_pTransformCom->Get_Distance(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

    if (m_fDistance < 30.f)
        m_bAggro = true;

    if(STATE_ATTACK_COMBO != m_iState && STATE_ATTACK_SLASH != m_iState )
        m_bAttackActive = false;

    if (true == m_bAggro)
    {
        if (STATE_IDLE == m_iState || STATE_WALK == m_iState)
            m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION), 0.1f);

        if (STATE_IDLE == m_iState || STATE_WALK == m_iState)
        {
            if (m_fAttackTime < m_fAttackDelay)
            {
                _float fRandom = m_pGameInstance->Get_Random_Normal();
                if (m_fDistance < 5.f)
                    m_iState = STATE_DASH_B;
                else if (fRandom < 0.5f)
                    m_iState = STATE_ATTACK_COMBO;
                else
                    m_iState = STATE_ATTACK_SLASH;
            }
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

    if (true == m_bAnimOver && STATE_DEATH != m_iState)
    {
        m_iState = STATE_IDLE;
        m_fAttackTime = m_pGameInstance->Get_Random(1.f, 3.f);
        m_fAttackDelay = 0.f;
        m_bAnimOver = false;
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

void CLab_Mage::Late_Update(_float fTimeDelta)
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

HRESULT CLab_Mage::Render()
{
    return S_OK;
}

void CLab_Mage::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
    __super::Intersect(strColliderTag, pCollisionObject, vSourInterval, vDestInterval);

}

void CLab_Mage::Be_Damaged(_uint iDamage, _fvector vAttackPos)
{
    m_iHP -= iDamage;
    cout << m_iHP << endl;


    _vector vDir = vAttackPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

    vDir = XMVector3Normalize(vDir);
    vLook = XMVector3Normalize(vLook);

    _float fDot = XMVectorGetX(XMVector3Dot(vDir, vLook));

    _float fRadian = acos(fDot);

    if (0.f > XMVector3Cross(vDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK)).m128_f32[1])
        fRadian *= -1.f;

    CLab_Mage_Body* pBody = static_cast<CLab_Mage_Body*>(m_Parts[PART_BODY]);
    pBody->Set_HittedDesc(XMConvertToDegrees(fRadian), iDamage);

    if (STATE_IMPACT == m_iState)
        pBody->Reset_Animation();
    else
        m_iState = STATE_IMPACT;
}

HRESULT CLab_Mage::Ready_Components()
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

HRESULT CLab_Mage::Ready_PartObjects()
{
    m_Parts.resize(PART_END);

    CLab_Mage_Body::BODY_DESC BodyDesc = {};
    BodyDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.fSpeedPerSec = 1.f;
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    BodyDesc.pMage_TransformCom = m_pTransformCom;
    BodyDesc.pNavigationCom = m_pNavigationCom;

    BodyDesc.pState = &m_iState;
    BodyDesc.pIsFinished = &m_isFinished;
    BodyDesc.pHP = &m_iHP;
    BodyDesc.pDistance = &m_fDistance;
    BodyDesc.pAnimStart = &m_bAnimStart;
    BodyDesc.pAnimOver = &m_bAnimOver;
    BodyDesc.pAttackActive = &m_bAttackActive;
    BodyDesc.pNoiseTextureCom = m_pNoiseTextureCom;
    BodyDesc.pRatio = &m_fRatio;

    if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Lab_Mage_Body"), &BodyDesc)))
        return E_FAIL;

    CLab_Mage_Attack::ATTACK_DESC AttackDesc = {};
    AttackDesc.fRotationPerSec = 0.f;
    AttackDesc.fSpeedPerSec = 0.f;
    AttackDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    AttackDesc.pSocketMatrix = dynamic_cast<CLab_Mage_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LM_Weapon_Sword");
    AttackDesc.vAngles = { 0.f, 0.f, 0.f };
    AttackDesc.vCenter = { 0.f, -1.f, 0.f };
    AttackDesc.vExtents = { 0.75f, 2.f, 0.75f };
    AttackDesc.pAttackActive = &m_bAttackActive;
    AttackDesc.iDamage = 10;

    if (FAILED(__super::Add_PartObject(PART_ATTACK, TEXT("Prototype_GameObject_Lab_Mage_Attack"), &AttackDesc)))
        return E_FAIL;

    CParticle_Monster_Death::PARTICLE_DEATH_DESC DeathDesc = {};
    DeathDesc.fRotationPerSec = 0.f;
    DeathDesc.fSpeedPerSec = 0.f;
    DeathDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    DeathDesc.pSocketMatrix = static_cast<CLab_Mage_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LM_Spine1");;
    DeathDesc.iNumInstance = 1000;
    DeathDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);
    DeathDesc.vRange = _float3(2.f, 2.f, 2.f);
    DeathDesc.vExceptRange = _float3(0.f, 0.f, 0.f);
    DeathDesc.vSize = _float2(0.15f, 0.3f);
    DeathDesc.vSpeed = _float2(1.5f, 3.f);
    DeathDesc.vLifeTime = _float2(1.f, 2.f);;

    if (FAILED(__super::Add_PartObject(PART_EFFECT_DEATH, TEXT("Prototype_GameObject_Particle_Monster_Death"), &DeathDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLab_Mage::Add_SpawnParticle(_fvector vPos, _float fOffset)
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
