#include "stdafx.h"
#include "Particle_LaunchWaterDrop.h"
#include "GameInstance.h"

CParticle_LaunchWaterDrop::CParticle_LaunchWaterDrop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CParticle_LaunchWaterDrop::CParticle_LaunchWaterDrop(const CParticle_LaunchWaterDrop& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CParticle_LaunchWaterDrop::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle_LaunchWaterDrop::Initialize(void* pArg)
{
    PARTICLE_LAUNCHWATERDROP_DESC* pDesc = static_cast<PARTICLE_LAUNCHWATERDROP_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_vPivot = pDesc->vPivot;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPos));
    m_fSpeed = 1.f;

    return S_OK;
}

_uint CParticle_LaunchWaterDrop::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CParticle_LaunchWaterDrop::Update(_float fTimeDelta)
{
    if (true == m_pVIBufferCom->Spread(XMLoadFloat3(&m_vPivot), m_fSpeed, 10.f, false, fTimeDelta))
        m_bDead = true;
}

void CParticle_LaunchWaterDrop::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
    //m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticle_LaunchWaterDrop::Render()
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
    if (FAILED(m_pShaderCom->Begin(5)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_LaunchWaterDrop::Ready_Components()
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
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Launch_WaterDrop"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CParticle_LaunchWaterDrop* CParticle_LaunchWaterDrop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_LaunchWaterDrop* pInstance = new CParticle_LaunchWaterDrop(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CPartticle_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_LaunchWaterDrop::Clone(void* pArg)
{
    CParticle_LaunchWaterDrop* pInstance = new CParticle_LaunchWaterDrop(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CPartticle_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_LaunchWaterDrop::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
