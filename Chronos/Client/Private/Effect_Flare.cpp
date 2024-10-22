#include "stdafx.h"
#include "Effect_Flare.h"
#include "GameInstance.h"

CEffect_Flare::CEffect_Flare(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBlendObject(pDevice, pContext)
{
}

CEffect_Flare::CEffect_Flare(const CEffect_Flare& Prototype)
    : CBlendObject(Prototype)
{
}

HRESULT CEffect_Flare::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Flare::Initialize(void* pArg)
{
    FLARE_DESC* pDesc = static_cast<FLARE_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPos));
    m_vColor = pDesc->vColor;
    m_vScale = pDesc->vScale;
    m_vStartScale = m_vScale;

    m_pTransformCom->Set_Scaled(m_vScale.x, m_vScale.y, m_vScale.z);

    return S_OK;
}

_uint CEffect_Flare::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CEffect_Flare::Update(_float fTimeDelta)
{
    _matrix ViewInverse = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
    m_pTransformCom->LookDir(ViewInverse.r[2]);

    m_pTransformCom->Set_Scaled(m_vScale.x, m_vScale.y, m_vScale.z);
    m_vScale.x -= fTimeDelta * m_vStartScale.x * 0.5f;
    m_vScale.y -= fTimeDelta * m_vStartScale.y * 0.5f;
    m_vScale.z -= fTimeDelta * m_vStartScale.z * 0.5f;

    m_vColor.w += fTimeDelta * 2.5f;

    if (0.f >= m_vScale.x || 0.f >= m_vScale.y || 0.f >= m_vScale.z)
    {
        m_bDead = true;
    }
}

void CEffect_Flare::Late_Update(_float fTimeDelta)
{
    if (FAILED(Compute_ViewZ()))
        return;

    m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Flare::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(4)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Flare::Ready_Components()
{
    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Flare"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer_Rect */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer_Pupple"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CEffect_Flare* CEffect_Flare::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_Flare* pInstance = new CEffect_Flare(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CEffect_Flare"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CEffect_Flare::Clone(void* pArg)
{
    CEffect_Flare* pInstance = new CEffect_Flare(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CEffect_Flare"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CEffect_Flare::Pooling()
{
    return new CEffect_Flare(*this);
}

void CEffect_Flare::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
