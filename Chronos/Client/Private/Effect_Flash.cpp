#include "stdafx.h"
#include "Effect_Flash.h"
#include "GameInstance.h"
#include "Particle_Smoke.h"

CEffect_Flash::CEffect_Flash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBlendObject(pDevice, pContext)
{
}

CEffect_Flash::CEffect_Flash(const CEffect_Flash& Prototype)
    : CBlendObject(Prototype)
{
}

HRESULT CEffect_Flash::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Flash::Initialize(void* pArg)
{
    FLASH_DESC* pDesc = static_cast<FLASH_DESC*>(pArg);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPos));
    m_pTransformCom->Set_Scaled(10.f, 10.f, 10.f);

    m_vDivide = _float2(6.f, 6.f);
    //m_vTeleportColor = _float4(0.294f, 0.f, 0.502f, 1.f);
    m_vTeleportColor = pDesc->vColor;


    return S_OK;
}

_uint CEffect_Flash::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CEffect_Flash::Update(_float fTimeDelta)
{
    _matrix ViewInverse = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
    m_pTransformCom->LookDir(ViewInverse.r[2]);
 
    m_fTexIndex += fTimeDelta * 45.f;

    if (m_fTexIndex > m_vDivide.x * m_vDivide.y - 1.f)
    {
        m_bDead = true;
    }
}

void CEffect_Flash::Late_Update(_float fTimeDelta)
{
    if(FAILED(Compute_ViewZ()))
        return;

    m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Flash::Render()
{
    _int iTexIndex = (_int)m_fTexIndex;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pTeleport_TextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL; 

    if (FAILED(m_pShaderCom->Bind_RawValue("g_TexDivide", &m_vDivide, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_iTexIndex", &iTexIndex, sizeof(_int))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vTeleportColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(2)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;


    return S_OK;
}

HRESULT CEffect_Flash::Ready_Components()
{
    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Teleport"),
        TEXT("Com_TeleportTexture"), reinterpret_cast<CComponent**>(&m_pTeleport_TextureCom))))
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

CEffect_Flash* CEffect_Flash::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_Flash* pInstance = new CEffect_Flash(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CEffect_Spark"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CEffect_Flash::Clone(void* pArg)
{
    CEffect_Flash* pInstance = new CEffect_Flash(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CEffect_Spark"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CEffect_Flash::Pooling()
{
    return new CEffect_Flash(*this);
}

void CEffect_Flash::Free()
{
    __super::Free();

    Safe_Release(m_pTeleport_TextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
