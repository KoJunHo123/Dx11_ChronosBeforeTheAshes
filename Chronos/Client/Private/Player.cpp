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

#include "Camera_Container.h"
#include "Camera_Shorder.h"
#include "Camera.h"

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
    PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
    m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
    m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));


    if (FAILED(Ready_Components(pDesc->iStartCellIndex)))
        return E_FAIL;

    if (FAILED(Ready_Parts()))
        return E_FAIL;

    if (FAILED(Ready_States()))
        return E_FAIL;

    m_pFSM->Set_State(STATE_MOVE);

    m_fSpeed = 4.f * 5.f;

    m_iMaxHP = 100;
    m_iHP = m_iMaxHP;

    m_iMaxStamina = 100;
    m_iStamina = m_iMaxStamina;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pNavigationCom->Get_CellCenterPos(pDesc->iStartCellIndex));

    // 임시. 수정 위치로 나중에 변경.
    XMStoreFloat3(&m_vSavePos, Get_Position());

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
            m_iHP = m_iMaxHP;

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
                }
            }
        }


        m_fRatio = 0.f;
        m_pColliderCom->Set_OnCollision(true);
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

        if (PLAYER_ACTION_DRAGONHEART == m_ePlayerAnim && 67 == m_iKeyFrameIndex)
        {
            m_iHP = m_iMaxHP;
            // 이펙트든 파티클이든 아무거나......
        }

        else if (PLAYER_ACTION_DRAGONSTONE == m_ePlayerAnim && 26 == m_iKeyFrameIndex)
        {
            // 
        }
        
    }
}

void CPlayer::Late_Update(_float fTimeDelta)
{	
    if (CCamera_Container::CAMERA_SHORDER == m_pCurrentCamera->Get_CameraIndex())
    {
        m_pFSM->Late_Update(fTimeDelta);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

        if (m_iHP <= 0)
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

_bool CPlayer::Be_Damaged(_uint iDamage, _fvector vAttackPos)
{
    if(false == m_bNonIntersect && 0 < m_iHP)
    {
        _vector vDir = vAttackPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

        vDir = XMVector3Normalize(vDir);
        vLook = XMVector3Normalize(vLook);

        _float fDot = XMVectorGetX(XMVector3Dot(vDir, vLook));

        if (STATE_BLOCK == m_pFSM->Get_State())
        {
            if (0.7 < fDot)
            {
                if (PLAYER_BLOCK_IMPACT == m_ePlayerAnim)
                    static_cast<CPlayer_Body*>(m_Parts[PART_BODY])->Reset_Animation();
                else
                    static_cast<CPlayer_Block*>(m_pFSM->Get_State(STATE_BLOCK))->Be_Impacted();

                return false;
            }
        }

        m_iHP -= iDamage;

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

    if(FAILED(m_pFSM->Add_State(CPlayer_Move::Create(&MoveDesc))))
        return E_FAIL;
    if(FAILED(m_pFSM->Add_State(CPlayer_Attack::Create(&desc))))
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
    JumpDesc.pHP = &m_iHP;
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
    BlockDesc.pStamina = &m_iStamina;

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
    ImpactDesc.pHP = &m_iHP;

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
}
