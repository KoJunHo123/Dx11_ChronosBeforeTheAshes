#include "stdafx.h"
#include "Effect_Spark.h"
#include "GameInstance.h"

CEffect_Spark::CEffect_Spark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBlendObject(pDevice, pContext)
{
}

CEffect_Spark::CEffect_Spark(const CEffect_Spark& Prototype)
    : CBlendObject(Prototype)
{
}

HRESULT CEffect_Spark::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Spark::Initialize(void* pArg)
{
    SPARK_DESC* pDesc = static_cast<SPARK_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPos));

    m_vColor = pDesc->vColor;
    m_fIndexSpeed = pDesc->fIndexSpeed;
    m_vScale = pDesc->vScale;
    m_vDivide = _float2(6.f, 6.f);
    
    return S_OK;
}

_uint CEffect_Spark::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CEffect_Spark::Update(_float fTimeDelta)
{
    _matrix ViewInverse = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
    m_pTransformCom->LookDir(ViewInverse.r[2]);

    m_fTexIndex += fTimeDelta * m_fIndexSpeed;

    _float fRatio = 1.f - (m_fTexIndex / (m_vDivide.x * m_vDivide.y - 1.f));
    m_pTransformCom->Set_Scaled(m_vScale.x * fRatio, m_vScale.y * fRatio, m_vScale.z * fRatio);

    if (m_fTexIndex > m_vDivide.x * m_vDivide.y - 1.f)
    {
        m_bDead = true;
    }
}

void CEffect_Spark::Late_Update(_float fTimeDelta)
{
    if (FAILED(Compute_ViewZ(g_strTransformTag)))
        return;

    m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Spark::Render()
{
    _int iTexIndex = (_int)m_fTexIndex;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_TexDivide", &m_vDivide, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_iTexIndex", &iTexIndex, sizeof(_int))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(3)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Spark::Ready_Components()
{
    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Spark"),
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

CEffect_Spark* CEffect_Spark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_Spark* pInstance = new CEffect_Spark(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CEffect_Spark"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CEffect_Spark::Clone(void* pArg)
{
    CEffect_Spark* pInstance = new CEffect_Spark(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CEffect_Spark"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CEffect_Spark::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
