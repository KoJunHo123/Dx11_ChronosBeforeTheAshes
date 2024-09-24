#include "stdafx.h"
#include "Particle_Smoke.h"
#include "GameInstance.h"

CParticle_Smoke::CParticle_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CBlendObject(pDevice, pContext)
{
}

CParticle_Smoke::CParticle_Smoke(const CParticle_Smoke& Prototype)
    : CBlendObject(Prototype)
{
}

HRESULT CParticle_Smoke::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle_Smoke::Initialize(void* pArg)
{
    SMOKE_DESC* pDesc = static_cast<SMOKE_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPos));

    m_fSpeed = 1.f;
    m_vScale = _float3(10.f, 10.f, 10.f);

    if (m_pGameInstance->Get_Random(-1.f, 1.f) < 0)
        m_iTurnDir = -1;
    else
        m_iTurnDir = 1;

   

    return S_OK;
}

_uint CParticle_Smoke::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CParticle_Smoke::Update(_float fTimeDelta)
{
    _vector vCamPos = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW).r[3];

    vCamPos = XMVector3TransformCoord(vCamPos, m_pTransformCom->Get_WorldMatrix_Inverse());

    m_fRatio += fTimeDelta;
    m_fSpeed -= fTimeDelta * 0.5f;

    if (m_fSpeed < 0.f)
        m_fSpeed = 0.f;

    // 버퍼로 퍼지면서, 커지면서, 돌아
    m_pVIBufferCom->Spread_Dir(XMVectorSet(0.f, 0.f, 0.f, 1.f), vCamPos, m_fSpeed, fTimeDelta);
    m_pVIBufferCom->Scaling(XMLoadFloat3(&m_vScale), fTimeDelta);
}

void CParticle_Smoke::Late_Update(_float fTimeDelta)
{
    if (FAILED(Compute_ViewZ(g_strTransformTag)))
        return;

    m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);

    if (m_fRatio > 1.f)
        m_bDead = true;

}

HRESULT CParticle_Smoke::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if(FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &m_fRatio, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(1)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Smoke::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Smoke"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    CVIBuffer_Instancing::INSTANCE_DESC desc = {};
    desc.iNumInstance = 10;
    desc.isLoop = false;
    desc.vCenter = _float3(0.f, 0.f, 0.f);
    desc.vRange = _float3(2.f, 2.f, 2.f);
    desc.vExceptRange = _float3(0.0f, 0.0f, 0.f);
    desc.vLifeTime = _float2(0.5f, 1.f);
    desc.vMaxColor = _float4(1.f, 1.f, 1.f, 1.f);
    desc.vMinColor = _float4(0.f, 0.f, 0.f, 1.f);
    desc.vSize = _float2(1.f, 1.f);
    desc.vSpeed = _float2(1.f, 1.f);

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &desc)))
        return E_FAIL;

    return S_OK;
}

CParticle_Smoke* CParticle_Smoke::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Smoke* pInstance = new CParticle_Smoke(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CParticle_Smoke"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Smoke::Clone(void* pArg)
{
    CParticle_Smoke* pInstance = new CParticle_Smoke(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CParticle_Smoke"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Smoke::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
