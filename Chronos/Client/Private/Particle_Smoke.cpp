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

    if (FAILED(Ready_Components(*pDesc)))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPos));
    m_vScale = pDesc->vScale;

    m_fSpeed = 1.f;
    m_vColor = pDesc->vColor;
    //m_vColor = ;
   

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


    // _vector vCamDir = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW).r[2];
    // 버퍼로 퍼지면서, 커지면서, 돌아
    if (true == m_pVIBufferCom->Spread_Dir(XMVectorSet(0.f, 0.f, 0.f, 1.f), vCamPos, m_fSpeed, 0.f, false, fTimeDelta))
        m_bDead = true;
    m_pVIBufferCom->Scaling(XMLoadFloat3(&m_vScale), fTimeDelta);
}

void CParticle_Smoke::Late_Update(_float fTimeDelta)
{

    if (FAILED(Compute_ViewZ()))
        return;

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
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
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(2)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Smoke::Ready_Components(const SMOKE_DESC& Desc)
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Smoke"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* For. Prototype_Component_VIBuffer_Particle_Snow */
    CVIBuffer_Instancing::INSTANCE_DESC ParticleDesc = {};
    ZeroMemory(&ParticleDesc, sizeof ParticleDesc);

    ParticleDesc.iNumInstance = Desc.iNumInstance;
    ParticleDesc.vCenter = Desc.vCenter;
    ParticleDesc.vRange = Desc.vRange;
    ParticleDesc.vExceptRange = Desc.vExceptRange;
    ParticleDesc.vLifeTime = Desc.vLifeTime;
    ParticleDesc.vMaxColor = Desc.vMaxColor;
    ParticleDesc.vMinColor = Desc.vMinColor;
    ParticleDesc.vSize = Desc.vSize;
    ParticleDesc.vSpeed = Desc.vSpeed;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &ParticleDesc)))
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
