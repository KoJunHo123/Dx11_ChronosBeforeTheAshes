#include "stdafx.h"
#include "Particle_LaunchStone.h"
#include "GameInstance.h"

CParticle_LaunchStone::CParticle_LaunchStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CParticle_LaunchStone::CParticle_LaunchStone(const CParticle_LaunchStone& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CParticle_LaunchStone::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_LaunchStone::Initialize(void* pArg)
{
    PARTICLE_LAUNCHSTONE_DESC* pDesc = static_cast<PARTICLE_LAUNCHSTONE_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_vPivot = pDesc->vPivot;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPos));
    m_fSpeed = 1.f;

    return S_OK;

	return S_OK;
}

_uint CParticle_LaunchStone::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
		return OBJ_DEAD;


	return OBJ_NOEVENT;
}

void CParticle_LaunchStone::Update(_float fTimeDelta)
{
    if (true == m_pVIBufferCom->Spread(XMLoadFloat3(&m_vPivot), m_fSpeed, 0.2f, false, fTimeDelta))
        m_bDead = true;
}

void CParticle_LaunchStone::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticle_LaunchStone::Render()
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
    if (FAILED(m_pShaderCom->Begin(4)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_LaunchStone::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Stone"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Launch_Stone"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

	return S_OK;
}

CParticle_LaunchStone* CParticle_LaunchStone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_LaunchStone* pInstance = new CParticle_LaunchStone(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CParticle_LaunchStone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_LaunchStone::Clone(void* pArg)
{
    CParticle_LaunchStone* pInstance = new CParticle_LaunchStone(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CParticle_LaunchStone"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_LaunchStone::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

}
