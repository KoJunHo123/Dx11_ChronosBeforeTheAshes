#include "stdafx.h"
#include "Particle_Spark.h"
#include "GameInstance.h"

CParticle_Spark::CParticle_Spark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBlendObject(pDevice, pContext)
{
}

CParticle_Spark::CParticle_Spark(const CParticle_Spark& Prototype)
    : CBlendObject(Prototype)
{
}

HRESULT CParticle_Spark::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle_Spark::Initialize(void* pArg)
{
    Particle_Spark_DESC* pDesc = static_cast<Particle_Spark_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_vPivot = pDesc->vPivot;
    //m_vPivot.y = -0.125f;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPos));
    m_fSpeed = 1.f;

    return S_OK;
}

_uint CParticle_Spark::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CParticle_Spark::Update(_float fTimeDelta)
{
    if (true == m_pVIBufferCom->Spread(XMLoadFloat3(&m_vPivot), m_fSpeed, fTimeDelta))
        m_bDead = true;

    //m_vPivot.y += fTimeDelta * 7.5f;
    //m_fSpeed -= fTimeDelta;
}

void CParticle_Spark::Late_Update(_float fTimeDelta)
{	
    if (FAILED(Compute_ViewZ(g_strTransformTag)))
        return;

    m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CParticle_Spark::Render()
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
    if (FAILED(m_pShaderCom->Begin(2)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Spark::Ready_Components()
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
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Spark"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CParticle_Spark* CParticle_Spark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Spark* pInstance = new CParticle_Spark(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CPartticle_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Spark::Clone(void* pArg)
{
    CParticle_Spark* pInstance = new CParticle_Spark(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CPartticle_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Spark::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
