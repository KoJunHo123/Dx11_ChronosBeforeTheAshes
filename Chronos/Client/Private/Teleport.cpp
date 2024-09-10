#include "stdafx.h"
#include "Teleport.h"
#include "GameInstance.h"

CTeleport::CTeleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteractionObject(pDevice, pContext)
{
}

CTeleport::CTeleport(const CTeleport& Prototype)
    : CInteractionObject(Prototype)
{
}

HRESULT CTeleport::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTeleport::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    TELEPORT_DESC* pDesc = static_cast<TELEPORT_DESC*>(pArg);
    m_vTeleportPos = pDesc->vTeleportPos;

    return S_OK;
}

_uint CTeleport::Priority_Update(_float fTimeDelta)
{
    return OBJ_NOEVENT;
}

void CTeleport::Update(_float fTimeDelta)
{
}

void CTeleport::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CTeleport::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }
    return S_OK;

    return S_OK;
}

void CTeleport::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
    if (TEXT("Coll_Player") == strColliderTag && m_pGameInstance->Get_DIKeyState_Down(DIKEYBOARD_E))
    {
        // 플레이어 상태 바꾸고
        // 플레이어 위치 바꾸고
    }
}

HRESULT CTeleport::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Teleport"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

CTeleport* CTeleport::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTeleport* pInstance = new CTeleport(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CTransform"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTeleport::Clone(void* pArg)
{
    CTeleport* pInstance = new CTeleport(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CTransform"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CTeleport::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}

