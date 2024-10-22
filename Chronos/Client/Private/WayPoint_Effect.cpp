#include "stdafx.h"
#include "WayPoint_Effect.h"
#include "GameInstance.h"

CWayPoint_Effect::CWayPoint_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{
}

CWayPoint_Effect::CWayPoint_Effect(const CWayPoint_Effect& Prototype)
    : CPartObject(Prototype)
{
}

HRESULT CWayPoint_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWayPoint_Effect::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

_uint CWayPoint_Effect::Priority_Update(_float fTimeDelta)
{
    return OBJ_NOEVENT;
}

void CWayPoint_Effect::Update(_float fTimeDelta)
{
    if(true == m_bOn)
    {
        XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));
        m_pVIBufferCom->Revolve(XMVectorSet(0.f, 0.f, 0.f, 1.f), _float3(0.f, XMConvertToRadians(30.f), 0.f), XMVectorSet(0.f, 0.4f, 0.f, 0.f), 1.f, true, fTimeDelta);
    }
}

void CWayPoint_Effect::Late_Update(_float fTimeDelta)
{
    if(true == m_bOn)
        m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}

HRESULT CWayPoint_Effect::Render()
{
    if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Begin(5)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CWayPoint_Effect::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Spark"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_WayPoint"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CWayPoint_Effect* CWayPoint_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWayPoint_Effect* pInstance = new CWayPoint_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CWayPoint_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWayPoint_Effect::Clone(void* pArg)
{
    CWayPoint_Effect* pInstance = new CWayPoint_Effect(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CWayPoint_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWayPoint_Effect::Pooling()
{
    return new CWayPoint_Effect(*this);
}

void CWayPoint_Effect::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
}
