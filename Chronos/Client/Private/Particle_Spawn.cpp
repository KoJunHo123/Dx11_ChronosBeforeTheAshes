#include "stdafx.h"
#include "Particle_Spawn.h"
#include "GameInstance.h"

#include "Monster.h"
#include "Particle_Smoke.h"


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
    m_iSpawnCellIndex = pDesc->iSpawnCellIndex;

    m_vPivot = _float3(0.f, 0.f, 0.f);
    m_fSpeed = 1.f;


    return S_OK;
}

_uint CParticle_Spawn::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    
    if (false == m_bConvergeOver)
    {
        m_bConvergeOver = m_pVIBufferCom_Converge->Converge(XMLoadFloat3(&m_vPivot), m_fSpeed, false, fTimeDelta);
    }

    if(true == m_bSpread)
    {
        m_pVIBufferCom_Spread->Spread(XMLoadFloat3(&m_vPivot), m_fSpeed, 0.f, false, fTimeDelta);
    }
    m_fSpawnTime += fTimeDelta;

    if (false == m_bSpread && 0.5f < m_fSpawnTime)
    {
        m_bSpread = true;
        Add_Monster();
        Add_ParticleSmoke();
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

    if (FAILED(m_pShaderCom->Begin(3)))
        return E_FAIL;

    if(false == m_bConvergeOver)
    {
        if (FAILED(m_pVIBufferCom_Converge->Bind_Buffers()))
            return E_FAIL;
        if (FAILED(m_pVIBufferCom_Converge->Render()))
            return E_FAIL;
    }
    if (true == m_bSpread)
    {
        if (FAILED(m_pVIBufferCom_Spread->Bind_Buffers()))
            return E_FAIL;
        if (FAILED(m_pVIBufferCom_Spread->Render()))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CParticle_Spawn::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_LightLong"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer_Converge */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Spawn_Converge"),
        TEXT("Com_VIBuffer_Converge"), reinterpret_cast<CComponent**>(&m_pVIBufferCom_Converge))))
        return E_FAIL;

    /* FOR.Com_VIBuffer_Spread */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Spawn_Spread"),
        TEXT("Com_VIBuffer_Spread"), reinterpret_cast<CComponent**>(&m_pVIBufferCom_Spread))))
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

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Troll"), &desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Spawn::Add_ParticleSmoke()
{
    CParticle_Smoke::SMOKE_DESC SmokeDesc = {};
    XMStoreFloat3(&SmokeDesc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    SmokeDesc.vScale = _float3(5.f, 5.f, 5.f);
    //SmokeDesc.vColor =_float4(0.541f, 0.169f, 0.886f, 1.f);
    SmokeDesc.vColor =_float4(0.f, 0.f, 0.f, 1.f);

    SmokeDesc.iNumInstance = 10;
    SmokeDesc.vCenter = _float3(0.f, 0.f, 0.f);
    SmokeDesc.vRange = _float3(2.f, 2.f, 2.f);
    SmokeDesc.vExceptRange = _float3(0.0f, 0.0f, 0.f);
    SmokeDesc.vLifeTime = _float2(1.f, 1.5f);
    SmokeDesc.vMaxColor = _float4(0.f, 0.f, 0.f, 1.f);
    SmokeDesc.vMinColor = _float4(1.f, 1.f, 1.f, 1.f);
    SmokeDesc.vSize = _float2(1.f, 2.f);
    SmokeDesc.vSpeed = _float2(1.f, 2.f);

    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Smoke"), &SmokeDesc);

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
    Safe_Release(m_pVIBufferCom_Converge);
    Safe_Release(m_pVIBufferCom_Spread);
}
