#include "stdafx.h"
#include "Effect_BloodCore.h"
#include "GameInstance.h"

#include "Effect_BloodSpray.h"

CEffect_BloodCore::CEffect_BloodCore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CEffect_BloodCore::CEffect_BloodCore(const CEffect_BloodCore& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CEffect_BloodCore::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_BloodCore::Initialize(void* pArg)
{
    BLOOD_DESC* pDesc = static_cast<BLOOD_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPos));
    m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);

    m_fIndexSpeed = 60.f;
    m_vDivide = _float2(5.f, 5.f);

    m_iCoreNum = (_int)m_pGameInstance->Get_Random(0.f, 2.f) % 2;

    CEffect_BloodSpray::BLOOD_DESC desc = {};
    desc.fRotationPerSec = pDesc->fRotationPerSec;
    desc.fSpeedPerSec = pDesc->fSpeedPerSec;
    desc.fIndexSpeed = m_fIndexSpeed;
    desc.vDir = pDesc->vDir;
    desc.vPos = pDesc->vPos;

    pDesc->vScale.x *= 0.5f;
    pDesc->vScale.y *= 2.f;
    desc.vScale = pDesc->vScale;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_BloodSpray"), &desc)))
        return E_FAIL;


    return S_OK;
}

_uint CEffect_BloodCore::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CEffect_BloodCore::Update(_float fTimeDelta)
{
    _matrix ViewInverse = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
    m_pTransformCom->LookDir(ViewInverse.r[2]);

    m_fTexIndex += fTimeDelta * m_fIndexSpeed;

    if (m_fTexIndex > m_vDivide.x * m_vDivide.y - 4.f)
    {
        m_bDead = true;
    }
}

void CEffect_BloodCore::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CEffect_BloodCore::Render()
{
    _int iTexIndex = (_int)m_fTexIndex;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pDiffTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", m_iCoreNum)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_TexDivide", &m_vDivide, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_iTexIndex", &iTexIndex, sizeof(_int))))
        return E_FAIL;

    //_float4 vColor = _float4(1.f, 0.f, 0.f, 1.f);
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
    //    return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(1)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_BloodCore::Ready_Components()
{
    /* FOR.Com_DiffTexture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BloodCore"),
        TEXT("Com_DiffTexture"), reinterpret_cast<CComponent**>(&m_pDiffTextureCom))))
        return E_FAIL;

    /* FOR.Com_NormTexture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BloodCore_N"),
        TEXT("Com_NormTexture"), reinterpret_cast<CComponent**>(&m_pNormTextureCom))))
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

CEffect_BloodCore* CEffect_BloodCore::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_BloodCore* pInstance = new CEffect_BloodCore(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CEffect_BloodCore"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_BloodCore::Clone(void* pArg)
{
    CEffect_BloodCore* pInstance = new CEffect_BloodCore(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CEffect_BloodCore"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_BloodCore::Pooling()
{
    return new CEffect_BloodCore(*this);
}

void CEffect_BloodCore::Free()
{
    __super::Free();

    Safe_Release(m_pDiffTextureCom);
    Safe_Release(m_pNormTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
