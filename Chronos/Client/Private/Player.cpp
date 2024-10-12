#include "stdafx.h"
#include "Player.h"
#include "GameInstance.h"

#include "Player_Action.h"
#include "Player_Attack.h"
#include "Player_Block.h"
#include "Player_Impact.h"
#include "Player_Jump.h"
#include "Player_Move.h"

#include "Player_Body.h"
#include "Player_Weapon.h"
#include "Player_Shield.h"
#include "Player_Item.h"
#include "Player_Skill.h"
#include "Player_Skill_Particle.h"
#include "Player_UseSkill_Particle.h"

#include "Camera_Container.h"
#include "Camera_Shorder.h"

#include "Inventory.h"
#include "DragonHeart.h"

#include "Trail_Revolve.h"
#include "Particle_DragonHeart.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CContainerObject( Prototype )
{
}

HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    m_fSpeed = 4.f * 5.f;
    m_fStartSpeed = m_fSpeed;

    m_fMaxHP = 100;
    m_fHP = m_fMaxHP;

    m_fMaxStamina = 100;
    m_fStamina = m_fMaxStamina;

    m_fMaxSkillGage = 100;
    m_fSkillGage = m_fMaxSkillGage;

    PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
    m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
    m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));

    if (FAILED(Ready_Inventory()))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc->iStartCellIndex)))
        return E_FAIL;

    if (FAILED(Ready_Parts()))
        return E_FAIL;

    if (FAILED(Ready_States()))
        return E_FAIL;

    m_pFSM->Set_State(STATE_MOVE);



    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pNavigationCom->Get_CellCenterPos(pDesc->iStartCellIndex));

    // 임시. 수정 위치로 나중에 변경.
    XMStoreFloat3(&m_vSavePos, Get_Position());

    m_fRatio = 0.f;
    m_HaveSkill[SKILL_RED] = true;
    m_HaveSkill[SKILL_PUPPLE] = true;

    m_pColliderCom->Set_OnCollision(true);
    return S_OK;
}

_uint CPlayer::Priority_Update(_float fTimeDelta)
{
    m_pCurrentCamera = dynamic_cast<CCamera*>(static_cast<CCamera_Container*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), 0))->Get_PartObject());

    if(CCamera_Container::CAMERA_SHORDER == m_pCurrentCamera->Get_CameraIndex())
    {
        if (true == m_bDead)
        {
            Set_Position(XMLoadFloat3(&m_vSavePos));
            m_pFSM->Set_State(STATE_MOVE);
            m_fHP = m_fMaxHP;

            m_bDead = false;
            m_bRevive = true;
        }

        if (true == m_bRevive)
        {
            m_fDeathDelay += fTimeDelta;
            if(1.f < m_fDeathDelay)
            {
                if(true==m_pGameInstance->FadeOut(fTimeDelta))
                {
                    m_fDeathDelay = 0.f;
                    m_bRevive = false;
                    m_fSkillGage = 0;
                }
            }
        }

        if (m_pGameInstance->Get_DIKeyState_Down(DIKEYBOARD_T) && m_fSkillDuration <= 0.f)
        {
            _uint iCurrentSkill = (_uint)m_eCurrentSkill;
            ++iCurrentSkill;
            if (true == m_HaveSkill[iCurrentSkill])
                m_eCurrentSkill = iCurrentSkill;

            if (SKILL_END == iCurrentSkill)
                m_eCurrentSkill = SKILL_RED;
        }

        m_pFSM->Priority_Update(fTimeDelta);
    }

    return OBJ_NOEVENT;
}

void CPlayer::Update(_float fTimeDelta)
{
    if (CCamera_Container::CAMERA_SHORDER == m_pCurrentCamera->Get_CameraIndex())
    {
        XMStoreFloat3(&m_vCameraLook, static_cast<CCamera_Shorder*>(m_pCurrentCamera)->Get_CameraLook());

        m_vCameraLook.y = 0.f;

        
        //if (1 == m_pNavigationCom->Get_CellType(m_pNavigationCom->Get_CurrentCellIndex()))
        //    m_pFSM->Set_State(STATE_JUMP);

        m_pFSM->Update(fTimeDelta);

        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

        //cout << m_pNavigationCom->Get_CurrentCellIndex() << endl;
        
        //_float3 vPos = {};
        //XMStoreFloat3(&vPos, Get_Position());
        //cout << vPos.x << endl;
        //cout << vPos.y << endl;
        //cout << vPos.z << endl << endl;

        Anim_Frame_Control();
    }

    _float4x4		ViewMatrix;
    _vector vPos = Get_Position();
    XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(XMVectorGetX(vPos), XMVectorGetY(vPos) + 100.f, XMVectorGetZ(vPos) -30.f, 1.f), 
        vPos, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

    m_pGameInstance->Set_Transform(CPipeLine::D3DTS_SHADOWVIEW, XMLoadFloat4x4(&ViewMatrix));
    
    _vector vTop = XMVectorSetY(vPos, XMVectorGetY(vPos) + 3.f);

}

void CPlayer::Late_Update(_float fTimeDelta)
{	
    if (CCamera_Container::CAMERA_SHORDER == m_pCurrentCamera->Get_CameraIndex())
    {
        m_pFSM->Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

        if (m_fHP <= 0)
        {
            if(STATE_JUMP == m_pFSM->Get_State())
            {
                if(true == m_pGameInstance->FadeIn(fTimeDelta))
                    m_bDead = true;
            }
            else if (STATE_IMPACT == m_pFSM->Get_State())
            {
                if (true == m_isFinished)
                {
                    if (true == m_pGameInstance->FadeIn(fTimeDelta))
                        m_bDead = true;
                }
            }
        }

        // 스킬 진행중
        if (0.f < m_fSkillDuration)
            m_fSkillDuration -= fTimeDelta;
        else
        {
            switch (m_eCurrentSkill)
            {
            case SKILL_RED:
                m_fSpeed = m_fStartSpeed;
                break;

            case SKILL_PUPPLE:
                m_bDrain = false;
                break;
            }
        }

        m_fStamina += fTimeDelta * 5.f;
        if (m_fStamina > m_fMaxStamina)
            m_fStamina = m_fMaxStamina;
        else if (m_fStamina < 0.f)
            m_fStamina = 0.f;

        m_fStamina = 100.f;
#ifdef _DEBUG
        m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
    }
}

HRESULT CPlayer::Render()
{
    if (CCamera_Container::CAMERA_SHORDER == m_pCurrentCamera->Get_CameraIndex())
    {
        if (FAILED(m_pFSM->Render()))
            return E_FAIL;
    }


    return S_OK;
}

void CPlayer::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{

}

_bool CPlayer::Be_Damaged(_float fDamage, _fvector vAttackPos)
{
    if(false == m_bNonIntersect && 0 < m_fHP)
    {
        _vector vDir = vAttackPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

        vDir = XMVector3Normalize(vDir);
        vLook = XMVector3Normalize(vLook);

        _float fDot = XMVectorGetX(XMVector3Dot(vDir, vLook));

        if (STATE_BLOCK == m_pFSM->Get_State())
        {
            m_fStamina -= fDamage;
            if (0.7 < fDot)
            {
                if (PLAYER_BLOCK_IMPACT == m_ePlayerAnim)
                    static_cast<CPlayer_Body*>(m_Parts[PART_BODY])->Reset_Animation();
                else
                    static_cast<CPlayer_Block*>(m_pFSM->Get_State(STATE_BLOCK))->Be_Impacted();

                return false;
            }
        }

        m_fHP -= fDamage;

        _float fRadian = acos(fDot);

        if (0.f > XMVector3Cross(vDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK)).m128_f32[1])
            fRadian *= -1.f;

        static_cast<CPlayer_Impact*>(m_pFSM->Get_State(CPlayer::STATE_IMPACT))->Set_HittedAngle(XMConvertToDegrees(fRadian));

        if (STATE_IMPACT == m_pFSM->Get_State() && false == m_bDead)
            static_cast<CPlayer_Body*>(m_Parts[PART_BODY])->Reset_Animation();
        else
            m_pFSM->Set_State(STATE_IMPACT);

        return true;
    }

    return false;
}

void CPlayer::Set_SavePos(_fvector vPos)
{
    _vector vLookDir = vPos - Get_Position();
    vLookDir = XMVectorSet(XMVectorGetX(vLookDir), 0.f, XMVectorGetZ(vLookDir), 1.f);
    vLookDir = XMVector3Normalize(vLookDir);
    m_pTransformCom->LookDir(vLookDir);

    m_pFSM->Set_State(STATE_ACTION);
    CPlayer_Action* pAction = static_cast<CPlayer_Action*>(m_pFSM->Get_State(STATE_ACTION));
    pAction->Set_State(CPlayer_Action::STATE_WAYPOINT);

    XMStoreFloat3(&m_vSavePos, Get_Position());
}

_bool CPlayer::Use_Runekey(_fvector vPos, _fvector vLookAt)
{
    if (nullptr == m_pInventory->Find_Item(TEXT("Item_RuneKey")))
        return false;

    _vector vPlayerPos = XMVectorSetY(vPos, XMVectorGetY(Get_Position()));
    _vector vAt = XMVectorSetY(vLookAt, XMVectorGetY(Get_Position()));

    Set_Position(vPlayerPos);
    m_pTransformCom->LookAt(vAt);
    m_pFSM->Set_State(STATE_ACTION);
    static_cast<CPlayer_Action*>(m_pFSM->Get_State(STATE_ACTION))->Set_State(CPlayer_Action::STATE_RUNEKEY);

    return true;
}

void CPlayer::Start_Teleport(_fvector vPos)
{
    m_pFSM->Set_State(STATE_ACTION);
    CPlayer_Action* pAction = static_cast<CPlayer_Action*>(m_pFSM->Get_State(STATE_ACTION));
    pAction->Set_State(CPlayer_Action::STATE_TELEPORT);
    pAction->Set_TargetPosition(vPos);
}

HRESULT CPlayer::Ready_Components(_int iStartCellIndex)
{
    /* FOR.Com_FSM */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_FSM"),
        TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFSM))))
        return E_FAIL;

    CNavigation::NAVIGATION_DESC desc;
    desc.iCurrentIndex = iStartCellIndex;

    /* FOR.Com_Navigation */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &desc)))
        return E_FAIL;

    /* For.Com_Collider_AABB */
    CBounding_AABB::BOUNDING_AABB_DESC			ColliderAABBDesc{};
    ColliderAABBDesc.vExtents = _float3(1.f, 2.f, 1.f);
    ColliderAABBDesc.vCenter = _float3(0.f, ColliderAABBDesc.vExtents.y, 0.f);

    CCollider::COLLIDER_DESC ColliderDesc = {};
    ColliderDesc.pOwnerObject = this;
    ColliderDesc.pBoundingDesc = &ColliderAABBDesc;
    ColliderDesc.strColliderTag = TEXT("Coll_Player");

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Noise"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pNoiseTextureCom), &desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_States()
{
    CPlayer_State::PLAYER_STATE_DESC desc{};
    desc.pFSM = m_pFSM;
    desc.pNavigationCom = m_pNavigationCom;
    desc.pTransformCom = m_pTransformCom;
    desc.Parts = &m_Parts;

    desc.pIsFinished = &m_isFinished;
    desc.pPlayerAnim = &m_ePlayerAnim;
    desc.pSpeed = &m_fSpeed;
    desc.pCameraLook = &m_vCameraLook;

    CPlayer_Move::PLAYER_STATE_MOVE_DESC MoveDesc{};
    MoveDesc.pFSM = m_pFSM;
    MoveDesc.pNavigationCom = m_pNavigationCom;
    MoveDesc.pTransformCom = m_pTransformCom;
    MoveDesc.Parts = &m_Parts;
    MoveDesc.pIsFinished = &m_isFinished;
    MoveDesc.pPlayerAnim = &m_ePlayerAnim;
    MoveDesc.pSpeed = &m_fSpeed;
    MoveDesc.pCameraLook = &m_vCameraLook;
    MoveDesc.pNonIntersect = &m_bNonIntersect;
    MoveDesc.fMaxSkillGage = m_fMaxSkillGage;
    MoveDesc.pInventory = m_pInventory;
    MoveDesc.pSkillGage = &m_fSkillGage;
    MoveDesc.pStamina = &m_fStamina;

    if(FAILED(m_pFSM->Add_State(CPlayer_Move::Create(&MoveDesc))))
        return E_FAIL;

    CPlayer_Attack::PLAYER_ATTACK_DESC AttackDesc{};
    AttackDesc.pFSM = m_pFSM;
    AttackDesc.pNavigationCom = m_pNavigationCom;
    AttackDesc.pTransformCom = m_pTransformCom;
    AttackDesc.Parts = &m_Parts;
    AttackDesc.pIsFinished = &m_isFinished;
    AttackDesc.pPlayerAnim = &m_ePlayerAnim;
    AttackDesc.pSpeed = &m_fSpeed;
    AttackDesc.pCameraLook = &m_vCameraLook;
    AttackDesc.pStamina = &m_fStamina;

    if(FAILED(m_pFSM->Add_State(CPlayer_Attack::Create(&AttackDesc))))
        return E_FAIL;


    CPlayer_Jump::PLAYER_STATE_JUMP_DESC JumpDesc{};
    JumpDesc.pFSM = m_pFSM;
    JumpDesc.pNavigationCom = m_pNavigationCom;
    JumpDesc.pTransformCom = m_pTransformCom;
    JumpDesc.Parts = &m_Parts;
    JumpDesc.pIsFinished = &m_isFinished;
    JumpDesc.pPlayerAnim = &m_ePlayerAnim;
    JumpDesc.pSpeed = &m_fSpeed;
    JumpDesc.pCameraLook = &m_vCameraLook;
    JumpDesc.pHP = &m_fHP;
    if(FAILED(m_pFSM->Add_State(CPlayer_Jump::Create(&JumpDesc))))
        return E_FAIL;

    CPlayer_Block::PLAYER_STATE_BLOCK_DESC BlockDesc = {};
    BlockDesc.pFSM = m_pFSM;
    BlockDesc.pNavigationCom = m_pNavigationCom;
    BlockDesc.pTransformCom = m_pTransformCom;
    BlockDesc.Parts = &m_Parts;
    BlockDesc.pIsFinished = &m_isFinished;
    BlockDesc.pPlayerAnim = &m_ePlayerAnim;
    BlockDesc.pSpeed = &m_fSpeed;
    BlockDesc.pCameraLook = &m_vCameraLook;
    BlockDesc.pStamina = &m_fStamina;

    if(FAILED(m_pFSM->Add_State(CPlayer_Block::Create(&BlockDesc))))
        return E_FAIL;

    CPlayer_Impact::PLAYER_STATE_IMPACT_DESC ImpactDesc = {};
    ImpactDesc.pFSM = m_pFSM;
    ImpactDesc.pNavigationCom = m_pNavigationCom;
    ImpactDesc.pTransformCom = m_pTransformCom;
    ImpactDesc.Parts = &m_Parts;
    ImpactDesc.pIsFinished = &m_isFinished;
    ImpactDesc.pPlayerAnim = &m_ePlayerAnim;
    ImpactDesc.pSpeed = &m_fSpeed;
    ImpactDesc.pCameraLook = &m_vCameraLook;
    ImpactDesc.pHP = &m_fHP;

    if (FAILED(m_pFSM->Add_State(CPlayer_Impact::Create(&ImpactDesc))))
        return E_FAIL;

    CPlayer_Action::PLAYER_ACTION_DESC ActionDesc = {};
    ActionDesc.pFSM = m_pFSM;
    ActionDesc.pNavigationCom = m_pNavigationCom;
    ActionDesc.pTransformCom = m_pTransformCom;
    ActionDesc.Parts = &m_Parts;
    ActionDesc.pIsFinished = &m_isFinished;
    ActionDesc.pPlayerAnim = &m_ePlayerAnim;
    ActionDesc.pSpeed = &m_fSpeed;
    ActionDesc.pCameraLook = &m_vCameraLook;
    ActionDesc.pPlayerColliderCom = m_pColliderCom;
    ActionDesc.pItemUsed = &m_bItemUsed;

    if (FAILED(m_pFSM->Add_State(CPlayer_Action::Create(&ActionDesc))))
        return E_FAIL;
    return S_OK;
}

HRESULT CPlayer::Ready_Parts()
{
    /* 실제 추가하고 싶은 파트오브젝트의 갯수만큼 밸류를 셋팅해놓자. */
    m_Parts.resize(PART_END);

    CPlayer_Part::PLAYER_PART_DESC desc = {};
    desc.fRotationPerSec = 90.f;
    desc.fSpeedPerSec = 1.f;
    desc.pFSM = m_pFSM;
    desc.pIsFinished = &m_isFinished;
    desc.pPlayerAnim = &m_ePlayerAnim;
    desc.pSpeed = &m_fSpeed;
    desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    desc.pFrameIndex = &m_iKeyFrameIndex;

    if(FAILED(Ready_Body(desc)))
        return E_FAIL;

    if(FAILED(Ready_Weapon(desc)))
        return E_FAIL;

    if (FAILED(Ready_Shield(desc)))
        return E_FAIL;

    if (FAILED(Ready_Item(desc)))
        return E_FAIL;

    if (FAILED(Ready_Skill()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_Inventory()
{
    m_pInventory = static_cast<CInventory*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Inventory"), 0));
    if (nullptr == m_pInventory)
        return E_FAIL;
    Safe_AddRef(m_pInventory);

    CDragonHeart::DRAGONHEART_DESC desc = {};
    desc.fRotationPerSec = XMConvertToRadians(90.f);
    desc.fSpeedPerSec = 1.f;

    CItem* pItem = static_cast<CItem*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_DragonHeart"), &desc));
    if (nullptr == pItem)
        return E_FAIL;

    m_pInventory->Add_Item(TEXT("Item_DragonHeart"), pItem);

    return S_OK;
}

HRESULT CPlayer::Ready_Body(const CPlayer_Part::PLAYER_PART_DESC& BaseDesc)
{
    CPlayer_Body::PLAYER_BODY_DESC desc = {};
    desc.fRotationPerSec = BaseDesc.fRotationPerSec;
    desc.fSpeedPerSec = BaseDesc.fSpeedPerSec;
    desc.pFSM = BaseDesc.pFSM;
    desc.pIsFinished = BaseDesc.pIsFinished;
    desc.pParentWorldMatrix = BaseDesc.pParentWorldMatrix;
    desc.pPlayerAnim = BaseDesc.pPlayerAnim;
    desc.pSpeed = BaseDesc.pSpeed;
    desc.pFrameIndex = BaseDesc.pFrameIndex;

    desc.pPlayerTransformCom = m_pTransformCom;
    desc.pNavigationCom = m_pNavigationCom;
    desc.pNoiseTextureCom = m_pNoiseTextureCom;
    desc.pRatio = &m_fRatio;

    if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Player_Body"), &desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_Weapon(const CPlayer_Part::PLAYER_PART_DESC& BaseDesc)
{
    CPlayer_Weapon::PLAYER_WEAPON_DESC desc = {};
    desc.fRotationPerSec = BaseDesc.fRotationPerSec;
    desc.fSpeedPerSec = BaseDesc.fSpeedPerSec;
    desc.pFSM = BaseDesc.pFSM;
    desc.pIsFinished = BaseDesc.pIsFinished;
    desc.pParentWorldMatrix = BaseDesc.pParentWorldMatrix;
    desc.pPlayerAnim = BaseDesc.pPlayerAnim;
    desc.pSpeed = BaseDesc.pSpeed;
    desc.pFrameIndex = BaseDesc.pFrameIndex;

    desc.pSocketBoneMatrix = dynamic_cast<CPlayer_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_M_Weapon_Sword");
    desc.pTailSocketMatrix = static_cast<CPlayer_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_M_Tail_Weapon_6");
    desc.pNoiseTextureCom = m_pNoiseTextureCom;
    desc.pRatio = &m_fRatio;
    desc.pDrain = &m_bDrain;
    desc.fMaxHP = m_fMaxHP;
    desc.pHP = &m_fHP;
    desc.pStamina = &m_fStamina;
    desc.pSkillGage = &m_fSkillGage;
    desc.fMaxSkillGage = m_fMaxSkillGage;
    desc.pSkillDuration = &m_fSkillDuration;

    if (FAILED(__super::Add_PartObject(PART_WEAPON, TEXT("Prototype_GameObject_Player_Weapon"), &desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_Shield(const CPlayer_Part::PLAYER_PART_DESC& BaseDesc)
{
    CPlayer_Shield::PLAYER_SHIELD_DESC desc = {};
    desc.fRotationPerSec = BaseDesc.fRotationPerSec;
    desc.fSpeedPerSec = BaseDesc.fSpeedPerSec;
    desc.pFSM = BaseDesc.pFSM;
    desc.pIsFinished = BaseDesc.pIsFinished;
    desc.pParentWorldMatrix = BaseDesc.pParentWorldMatrix;
    desc.pPlayerAnim = BaseDesc.pPlayerAnim;
    desc.pSpeed = BaseDesc.pSpeed;
    desc.pFrameIndex = BaseDesc.pFrameIndex;

    desc.pSocketBoneMatrix = dynamic_cast<CPlayer_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_M_Weapon_Shield");
    desc.pNoiseTextureCom = m_pNoiseTextureCom;
    desc.pRatio = &m_fRatio;

    if (FAILED(__super::Add_PartObject(PART_SHIELD, TEXT("Prototype_GameObject_Player_Shield"), &desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_Item(const CPlayer_Part::PLAYER_PART_DESC& BaseDesc)
{
    CPlayer_Item::PLAYER_ITEM_DESC desc = {};
    desc.fRotationPerSec = BaseDesc.fRotationPerSec;
    desc.fSpeedPerSec = BaseDesc.fSpeedPerSec;
    desc.pFSM = BaseDesc.pFSM;
    desc.pIsFinished = BaseDesc.pIsFinished;
    desc.pParentWorldMatrix = BaseDesc.pParentWorldMatrix;
    desc.pPlayerAnim = BaseDesc.pPlayerAnim;
    desc.pSpeed = BaseDesc.pSpeed;
    desc.pFrameIndex = BaseDesc.pFrameIndex;

    desc.pSocketBoneMatrix = dynamic_cast<CPlayer_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_M_Item");
    desc.pInventory = m_pInventory;

    if (FAILED(__super::Add_PartObject(PART_ITEM, TEXT("Prototype_GameObject_Player_Item"), &desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_Skill()
{
    CPlayer_Skill::PLAYER_SKILL_DESC SkillDesc = {};

    SkillDesc.fRotationPerSec = XMConvertToRadians(90.f);
    SkillDesc.fSpeedPerSec = 1.f;
    SkillDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    SkillDesc.pSocketBoneMatrix = dynamic_cast<CPlayer_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_M_Weapon_Sword");
    SkillDesc.pTailSocketMatrix = static_cast<CPlayer_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_M_Tail_Weapon_6");
    SkillDesc.pCurrentSkill = &m_eCurrentSkill;
    SkillDesc.pSkilDuration = &m_fSkillDuration;

    if (FAILED(__super::Add_PartObject(PART_SKILL, TEXT("Prototype_GameObject_Player_Skill"), &SkillDesc)))
        return E_FAIL;

    CPlayer_Skill_Particle::PLAYER_SKILL_DESC ParticleDesc = {};
    ParticleDesc.fRotationPerSec = XMConvertToRadians(90.f);
    ParticleDesc.fSpeedPerSec = 1.f;
    ParticleDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    ParticleDesc.pSocketBoneMatrix = dynamic_cast<CPlayer_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_M_Weapon_Sword");
    ParticleDesc.pTailSocketMatrix = static_cast<CPlayer_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_M_Tail_Weapon_6");
    ParticleDesc.pCurrentSkill = &m_eCurrentSkill;
    ParticleDesc.pSkilDuration = &m_fSkillDuration;

    if (FAILED(__super::Add_PartObject(PART_SKILL_PARTICLE, TEXT("Prototype_GameObject_Player_Skill_Particle"), &ParticleDesc)))
        return E_FAIL;

    CPlayer_UseSkill_Particle::PLAYER_USESKILL_DESC UseSkillDesc = {};
    UseSkillDesc.fRotationPerSec = XMConvertToRadians(90.f);
    UseSkillDesc.fSpeedPerSec = 1.f;
    UseSkillDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    UseSkillDesc.pSocketBoneMatrix = dynamic_cast<CPlayer_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_M_Hand_L");
    UseSkillDesc.pCurrentSkill = &m_eCurrentSkill;
    UseSkillDesc.pSkilDuration = &m_fSkillDuration;

    if (FAILED(__super::Add_PartObject(PART_USESKILL_PARTICLE, TEXT("Prototype_GameObject_Player_UseSkill_Particle"), &UseSkillDesc)))
        return E_FAIL;

    return S_OK;
}

void CPlayer::Anim_Frame_Control()
{
    if (PLAYER_ACTION_DRAGONHEART == m_ePlayerAnim )
    {
        if (30 <= m_iKeyFrameIndex && m_iKeyFrameIndex < 40)
        {
            CPlayer_Item* pItem = static_cast<CPlayer_Item*>(m_Parts[CPlayer::PART_ITEM]);
            if(true == pItem->IsEmpty())
            {
                pItem->Set_Item(TEXT("Item_DragonHeart"));
            }
        }
        else if(false == m_bItemUsed && 67 <= m_iKeyFrameIndex)
        {
            m_bItemUsed = true;
            static_cast<CDragonHeart*>(m_pInventory->Find_Item(TEXT("Item_DragonHeart")))->Use_Item(this);
            static_cast<CPlayer_Item*>(m_Parts[PART_ITEM])->Release_Item();
            Add_TrailRevolve();
            Add_Particle_DragonHeart();
        }
    }
    else if (PLAYER_ACTION_DRAGONSTONE == m_ePlayerAnim && 26 <= m_iKeyFrameIndex)
    {
        if (m_fSkillDuration <= 0.f)
        {
            switch (m_eCurrentSkill)
            {
            case SKILL_RED:
                m_fSpeed *= 1.5f;
                break;

            case SKILL_PUPPLE:
                m_bDrain = true;
                break;
            }
            //m_fSkillGage = 0.f;
            m_fSkillGage = 100.f;
            m_fSkillDuration = 10.f;
            static_cast<CPlayer_UseSkill_Particle*>(m_Parts[PART_USESKILL_PARTICLE])->Set_On(true);
        }

    }
    else if (PLAYER_ACTION_RUNEKEYHOLE == m_ePlayerAnim)
    {
        CPlayer_Item* pItem = static_cast<CPlayer_Item*>(m_Parts[CPlayer::PART_ITEM]);

        if (28 <= m_iKeyFrameIndex && m_iKeyFrameIndex < 38)
        {
            if (true == pItem->IsEmpty())
            {
                pItem->Set_Item(TEXT("Item_RuneKey"));
            }
        }
        else if (false == m_bItemUsed && 70 <= m_iKeyFrameIndex)
        {
            m_bItemUsed = true;
        }
        else if (180 <= m_iKeyFrameIndex)
        {
            if (false == pItem->IsEmpty())
            {
                static_cast<CPlayer_Item*>(m_Parts[PART_ITEM])->Release_Item();
            }
        }

    }
}

HRESULT CPlayer::Add_TrailRevolve()
{
    CTrail_Revolve::TRAIL_REVOLOVE_DESC desc = {};

    desc.iTrailCount = 10;
    desc.vColor = _float4(1.0f, 0.271f, 0.0f, 1.f);
    XMStoreFloat3(&desc.vPos, Get_Position());
    desc.vRange = _float3(4.f, 4.f, 4.f);
    desc.fAccel = 5.f;
    desc.fSpeed = 10.f;
    desc.fScale = 0.3f;
    desc.fRotaionPerSecond = XMConvertToRadians(360.f);
    desc.eType = CTrail_Revolve::TYPE_CONVERGE;
    desc.fTypeAccel = 0.03f;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Trail"), TEXT("Prototype_GameObject_Trail_Revolve"), &desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Add_Particle_DragonHeart()
{
    CParticle_DragonHeart::PARTICLE_DRAGONHEART_DESC desc = {};

    desc.fRotationPerSec = XMConvertToRadians(90.f);
    desc.fSpeedPerSec = 1.f;
    XMStoreFloat3(&desc.vPos, static_cast<CPlayer_Item*>(m_Parts[PART_ITEM])->Get_Position());

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_DragonHeart"), &desc)))
        return E_FAIL;

    return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
    __super::Free();

    Safe_Release(m_pFSM);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pNoiseTextureCom);
    Safe_Release(m_pInventory);
}
