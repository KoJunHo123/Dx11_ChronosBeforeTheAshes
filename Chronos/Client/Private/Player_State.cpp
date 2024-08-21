#include "stdafx.h"
#include "Player_State.h"
#include "GameInstance.h"

#include "Player.h"
#include "PartObject.h"


CPlayer_State::CPlayer_State()
{
}

HRESULT CPlayer_State::Initialize(void* pArg)
{
    PLAYER_STATE_DESC* pDesc = static_cast<PLAYER_STATE_DESC*>(pArg);

    __super::Initialize(pArg);

    m_pTransformCom = pDesc->pTransformCom;
    Safe_AddRef(m_pTransformCom);
    
    m_Parts = *pDesc->Parts;

    m_pSpeed = pDesc->pSpeed;
    m_pPlayerAnim = pDesc->pPlayerAnim;
    m_pIsFinished = pDesc->pIsFinished;

    return S_OK;
}

HRESULT CPlayer_State::StartState(void** pArg)
{
    if (nullptr == pArg)
        return E_FAIL;


    return S_OK;
}

void CPlayer_State::Priority_Update(_float fTimeDelta)
{
    for (auto& pPartObject : m_Parts)
    {
        if (nullptr == pPartObject)
            continue;
        pPartObject->Priority_Update(fTimeDelta);
    }

}

void CPlayer_State::Update(_float fTimeDelta)
{
    for (auto& pPartObject : m_Parts)
    {
        if (nullptr == pPartObject)
            continue;
        pPartObject->Update(fTimeDelta);
    }
}

void CPlayer_State::Late_Update(_float fTimeDelta)
{
    for (auto& pPartObject : m_Parts)
    {
        if (nullptr == pPartObject)
            continue;
        pPartObject->Late_Update(fTimeDelta);
    }
}

HRESULT CPlayer_State::Render()
{
    return S_OK;
}

HRESULT CPlayer_State::ExitState(void** pArg)
{
    return S_OK;
}



void CPlayer_State::Look_CameraDir()
{
    CTransform* pCameraTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), g_strTransformTag));

    _vector vCameraLook = pCameraTransform->Get_State(CTransform::STATE_LOOK);
    m_pTransformCom->LookDir(vCameraLook);

}

void CPlayer_State::Free()
{
    __super::Free();

    Safe_Release(m_pTransformCom);
}
