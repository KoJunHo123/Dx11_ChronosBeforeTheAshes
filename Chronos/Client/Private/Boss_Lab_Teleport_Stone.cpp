#include "stdafx.h"
#include "Boss_Lab_Teleport_Stone.h"
#include "GameInstance.h"

CBoss_Lab_Teleport_Stone::CBoss_Lab_Teleport_Stone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{
}

CBoss_Lab_Teleport_Stone::CBoss_Lab_Teleport_Stone(const CBoss_Lab_Teleport_Stone& Prototype)
    : CPartObject(Prototype)
{
}

HRESULT CBoss_Lab_Teleport_Stone::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBoss_Lab_Teleport_Stone::Initialize(void* pArg)
{
    BOSSTELEPORT_STONE_DESC* pDesc = static_cast<BOSSTELEPORT_STONE_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;
    
    m_vPivot = { 0.f, -8.f, 0.f };
    m_fSpeed = pDesc->fSpeed;
    m_fGravity = pDesc->fGravity;

    return S_OK;
}

_uint CBoss_Lab_Teleport_Stone::Priority_Update(_float fTimeDelta)
{
    return OBJ_NOEVENT;
}

void CBoss_Lab_Teleport_Stone::Update(_float fTimeDelta)
{
    if (true == m_bOn)
    {
        if (true == m_pVIBufferCom->Spread(XMLoadFloat3(&m_vPivot), m_fSpeed, m_fGravity, false, fTimeDelta))
        {
            m_bOn = false;
            m_pVIBufferCom->Reset();
        }
    }
}

void CBoss_Lab_Teleport_Stone::Late_Update(_float fTimeDelta)
{
    if (true == m_bOn)
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CBoss_Lab_Teleport_Stone::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(4)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBoss_Lab_Teleport_Stone::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Stone"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Boss_Teleport_Stone"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CBoss_Lab_Teleport_Stone* CBoss_Lab_Teleport_Stone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBoss_Lab_Teleport_Stone* pInstance = new CBoss_Lab_Teleport_Stone(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CBoss_Lab_Teleport_Stone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoss_Lab_Teleport_Stone::Clone(void* pArg)
{
    CBoss_Lab_Teleport_Stone* pInstance = new CBoss_Lab_Teleport_Stone(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CBoss_Lab_Teleport_Stone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_Lab_Teleport_Stone::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
