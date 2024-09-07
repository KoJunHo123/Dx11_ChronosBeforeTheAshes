#include "stdafx.h"
#include "Player_Part.h"
#include "GameInstance.h"


CPlayer_Part::CPlayer_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{pDevice, pContext}
{
}

CPlayer_Part::CPlayer_Part(const CPlayer_Part& Prototype)
    : CPartObject{Prototype}
{
}

HRESULT CPlayer_Part::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer_Part::Initialize(void* pArg)
{
    PLAYER_PART_DESC* pDesc = static_cast<PLAYER_PART_DESC*>(pArg);

    m_pFSM = pDesc->pFSM;
    m_pPlayerAnim = pDesc->pPlayerAnim;
    m_pIsFinished = pDesc->pIsFinished;
    m_pSpeed = pDesc->pSpeed;
    m_pFrameIndex = pDesc->pFrameIndex;

    Safe_AddRef(m_pFSM);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

_uint CPlayer_Part::Priority_Update(_float fTimeDelta)
{
    return OBJ_NOEVENT;
}

void CPlayer_Part::Update(_float fTimeDelta)
{
}

void CPlayer_Part::Late_Update(_float fTimeDelta)
{
}

HRESULT CPlayer_Part::Render()
{
    return S_OK;
}

void CPlayer_Part::Free()
{
    __super::Free();

    Safe_Release(m_pFSM);
}
