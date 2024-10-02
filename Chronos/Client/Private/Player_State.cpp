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
    m_pNavigationCom = pDesc->pNavigationCom;

    Safe_AddRef(m_pTransformCom);
    Safe_AddRef(m_pNavigationCom);

    m_Parts = *pDesc->Parts;

    m_pSpeed = pDesc->pSpeed;
    m_pPlayerAnim = pDesc->pPlayerAnim;
    m_pIsFinished = pDesc->pIsFinished;
    m_pCameraLook = pDesc->pCameraLook;

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

    m_pTransformCom->SetUp_OnCell(m_pNavigationCom);
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
    m_bMotionLock = false;

    return S_OK;
}

void CPlayer_State::Look_CameraDir()
{
    m_pTransformCom->LookDir(XMLoadFloat3(m_pCameraLook), 0.3f);
}

void CPlayer_State::Free()
{
    __super::Free();

    Safe_Release(m_pTransformCom);
    Safe_Release(m_pNavigationCom);
}
