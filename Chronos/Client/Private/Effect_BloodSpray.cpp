#include "stdafx.h"
#include "Effect_BloodSpray.h"
#include "GameInstance.h"

CEffect_BloodSpray::CEffect_BloodSpray(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CEffect_BloodSpray::CEffect_BloodSpray(const CEffect_BloodSpray& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CEffect_BloodSpray::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_BloodSpray::Initialize(void* pArg)
{
    BLOOD_DESC* pDesc = static_cast<BLOOD_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    _vector vPos = XMLoadFloat3(&pDesc->vPos);
    
    vPos += XMLoadFloat3(&pDesc->vDir) * (XMLoadFloat3(&pDesc->vScale) * 0.5f);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
    m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);

    m_fIndexSpeed = pDesc->fIndexSpeed;
    m_vMoveDir = pDesc->vDir;
    m_vDivide = _float2(6.f, 5.f);
    m_fSpeed = 5.f;

    m_iCoreNum = (_int)m_pGameInstance->Get_Random(0.f, 6.f) % 6;


    return S_OK;
}

_uint CEffect_BloodSpray::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CEffect_BloodSpray::Update(_float fTimeDelta)
{
    _vector vNormDir = XMVector3Normalize(XMLoadFloat3(&m_vMoveDir));
    _matrix ViewInverse = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
    _vector vCamDir = ViewInverse.r[3] - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    // 룩 기준으로 돌려.
    _float3		vScale = m_pTransformCom->Get_Scaled();

    _vector     vUp = vNormDir;
    _vector     vRight = XMVector3Cross(XMVector3Normalize(vCamDir), vUp);
    _vector     vLook = XMVector3Cross(vUp, vRight);

    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
    m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);

    m_fTexIndex += fTimeDelta * m_fIndexSpeed;

    if (m_fTexIndex > m_vDivide.x * m_vDivide.y - 4.f)
    {
        m_bDead = true;
    }
}

void CEffect_BloodSpray::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CEffect_BloodSpray::Render()
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

HRESULT CEffect_BloodSpray::Ready_Components()
{
    /* FOR.Com_DiffTexture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BloodSpray"),
        TEXT("Com_DiffTexture"), reinterpret_cast<CComponent**>(&m_pDiffTextureCom))))
        return E_FAIL;

    /* FOR.Com_NormTexture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BloodSpray_N"),
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

CEffect_BloodSpray* CEffect_BloodSpray::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_BloodSpray* pInstance = new CEffect_BloodSpray(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CEffect_BloodSpray"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_BloodSpray::Clone(void* pArg)
{
    CEffect_BloodSpray* pInstance = new CEffect_BloodSpray(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CEffect_BloodSpray"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_BloodSpray::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pDiffTextureCom);
    Safe_Release(m_pNormTextureCom);
}
