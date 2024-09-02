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

    //m_fSpeed = 5.f;
    m_fSpeed = 20.f;
    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    m_pFSM->Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
    XMStoreFloat3(&m_vCameraLook, static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), g_strTransformTag))->Get_State(CTransform::STATE_LOOK));

    m_vCameraLook.y = 0.f;

    m_pFSM->Update(fTimeDelta);

    m_pTransformCom->SetUp_OnCell(m_pNavigationCom);
}

void CPlayer::Late_Update(_float fTimeDelta)
{	
    m_pFSM->Late_Update(fTimeDelta);
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPlayer::Render()
{
    if (FAILED(m_pFSM->Render()))
        return E_FAIL;
    return S_OK;
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

    if(FAILED(m_pFSM->Add_State(CPlayer_Move::Create(&desc))))
        return E_FAIL;
    if(FAILED(m_pFSM->Add_State(CPlayer_Attack::Create(&desc))))
        return E_FAIL;
    if(FAILED(m_pFSM->Add_State(CPlayer_Jump::Create(&desc))))
        return E_FAIL;
    if(FAILED(m_pFSM->Add_State(CPlayer_Block::Create(&desc))))
        return E_FAIL;
    if(FAILED(m_pFSM->Add_State(CPlayer_Impact::Create(&desc))))
        return E_FAIL;
    if(FAILED(m_pFSM->Add_State(CPlayer_Action::Create(&desc))))
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


}
