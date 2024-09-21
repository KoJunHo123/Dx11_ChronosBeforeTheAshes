#include "stdafx.h"
#include "Particle_Spawn.h"
#include "GameInstance.h"

#include "Monster.h"


CParticle_Spawn::CParticle_Spawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBlendObject(pDevice, pContext)
{
}

CParticle_Spawn::CParticle_Spawn(const CParticle_Spawn& Prototype)
    : CBlendObject(Prototype)
{
}

HRESULT CParticle_Spawn::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle_Spawn::Initialize(void* pArg)
{
    PARTICLE_SPAWN_DESC* pDesc = static_cast<PARTICLE_SPAWN_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPos));
    m_eType = pDesc->eType;
    m_iSpawnCellIndex = pDesc->iSpawnCellIndex;

    m_vPivot = _float3(0.f, 0.f, 0.f);
    m_fSpeed = 1.f;

    return S_OK;
}

_uint CParticle_Spawn::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    m_fSpeed += fTimeDelta * 10.f;

    if (false == m_bSpawn)
    {
        _bool bPupple{ false }, bWhite{ false };
        bPupple = m_pVIBufferCom_Pupple->Spread(XMLoadFloat3(&m_vPivot), m_fSpeed, fTimeDelta);
        bWhite = m_pVIBufferCom_White->Spread(XMLoadFloat3(&m_vPivot), m_fSpeed, fTimeDelta);

        if (true == bPupple && true == bWhite)
        {
            Add_Monster();
            m_bSpawn = true;
        }
    }
    else
    {
        _bool bPupple{ false }, bWhite{ false };
        bPupple = m_pVIBufferCom_Pupple->Converge(XMLoadFloat3(&m_vPivot), m_fSpeed, fTimeDelta);
        bWhite = m_pVIBufferCom_White->Converge(XMLoadFloat3(&m_vPivot), m_fSpeed, fTimeDelta);

        if (true == bPupple && true == bWhite)
            m_bDead = true;
    }

    return OBJ_NOEVENT;
}

void CParticle_Spawn::Update(_float fTimeDelta)
{
}

void CParticle_Spawn::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticle_Spawn::Render()
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

    if (FAILED(m_pShaderCom->Begin(1)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom_Pupple->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom_Pupple->Render()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom_White->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom_White->Render()))
        return E_FAIL;
    return S_OK;
}

HRESULT CParticle_Spawn::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Spawn_Pupple"),
        TEXT("Com_VIBuffer_Pupple"), reinterpret_cast<CComponent**>(&m_pVIBufferCom_Pupple))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Spawn_White"),
        TEXT("Com_VIBuffer_White"), reinterpret_cast<CComponent**>(&m_pVIBufferCom_White))))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Spawn::Add_Monster()
{
    CMonster::MONSTER_DESC desc = {};
    desc.fRotationPerSec = XMConvertToRadians(90.f);
    desc.fSpeedPerSec = 1.f;
    desc.vRotation = {};
    desc.vScale = { 1.f, 1.f, 1.f };
    desc.iStartCellIndex = m_iSpawnCellIndex;

    if (-1 == desc.iStartCellIndex)
        return E_FAIL;

    switch (m_eType)
    {
    case TYPE_TROLL:
        if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Troll"), &desc)))
            return E_FAIL;
        break;

    case TYPE_MAGE:
        if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Mage"), &desc)))
            return E_FAIL;
        break;

    case TYPE_DRUM:
        if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Drum"), &desc)))
            return E_FAIL;
        break;

    case TYPE_CONSTRUCT:
        if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Construct"), &desc)))
            return E_FAIL;
        break;

    case TYPE_END:
        break;
    }
    

    return S_OK;
}

CParticle_Spawn* CParticle_Spawn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Spawn* pInstance = new CParticle_Spawn(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CPartticle_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Spawn::Clone(void* pArg)
{
    CParticle_Spawn* pInstance = new CParticle_Spawn(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CPartticle_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Spawn::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom_Pupple);
    Safe_Release(m_pVIBufferCom_White);
}
