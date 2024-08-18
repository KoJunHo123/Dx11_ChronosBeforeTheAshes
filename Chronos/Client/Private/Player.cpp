#include "stdafx.h"
#include "Player.h"
#include "GameInstance.h"

#include "Player_Action.h"
#include "Player_Attack.h"
#include "Player_Block.h"
#include "Player_Impact.h"
#include "Player_Jump.h"
#include "Player_Move.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CGameObject( Prototype )
{
}

HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    PLAYER_DESC* Desc = static_cast<PLAYER_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_States()))
        return E_FAIL;

    m_pFSM->Set_State(STATE_MOVE);

    m_fSpeed = 3.f;

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    m_pFSM->Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
    m_pFSM->Update(fTimeDelta);

    if (m_pGameInstance->Key_Pressing('T'))
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 0.05f);

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


HRESULT CPlayer::Load_Data(ifstream* pInFile)
{
    _matrix WorldMatrix = {};
    if (false == (_bool)pInFile->read(reinterpret_cast<_char*>(&WorldMatrix), sizeof(_matrix)))
        return E_FAIL;
    m_pTransformCom->Set_WorldMatrix(WorldMatrix);

    return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* FOR.Com_FSM */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_FSM"),
        TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFSM))))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_States()
{
    CPlayer_State::PLAYER_STATE_DESC desc{};
    desc.pFSM = m_pFSM;
    desc.pModelCom = m_pModelCom;
    desc.pShaderCom = m_pShaderCom;
    desc.pTransformCom = m_pTransformCom;
    desc.pSpeed = &m_fSpeed;
    
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

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pFSM);

}
