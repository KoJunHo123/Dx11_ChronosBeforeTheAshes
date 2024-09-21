#include "stdafx.h"
#include "Particle_Attack.h"
#include "GameInstance.h"

CParticle_Attack::CParticle_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBlendObject(pDevice, pContext)
{
}

CParticle_Attack::CParticle_Attack(const CParticle_Attack& Prototype)
    : CBlendObject(Prototype)
{
}

HRESULT CParticle_Attack::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle_Attack::Initialize(void* pArg)
{
    PARTICLE_ATTACK_DESC* pDesc = static_cast<PARTICLE_ATTACK_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc->eType)))
        return E_FAIL;

    m_vPivot = pDesc->vPivot;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPos));

    return S_OK;
}

_uint CParticle_Attack::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CParticle_Attack::Update(_float fTimeDelta)
{
    m_vPivot.y += fTimeDelta * 2.f;
    if (true == m_pVIBufferCom->Spread(XMLoadFloat3(&m_vPivot), 1.f, fTimeDelta))
        m_bDead = true;
}

void CParticle_Attack::Late_Update(_float fTimeDelta)
{	
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticle_Attack::Render()
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

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Attack::Ready_Components(TYPE eType)
{	
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    _wstring strProtytypeTag = {};
    switch (eType)
    {
    case TYPE_PLAYER:
        strProtytypeTag = TEXT("Prototype_Component_VIBuffer_Particle_Attack_Player");
        break;
    case TYPE_MONSTER:
        strProtytypeTag = TEXT("Prototype_Component_VIBuffer_Particle_Attack_Monster");
        break;
    }
        
    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, strProtytypeTag,
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CParticle_Attack* CParticle_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Attack* pInstance = new CParticle_Attack(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CPartticle_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Attack::Clone(void* pArg)
{
    CParticle_Attack* pInstance = new CParticle_Attack(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CPartticle_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Attack::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
