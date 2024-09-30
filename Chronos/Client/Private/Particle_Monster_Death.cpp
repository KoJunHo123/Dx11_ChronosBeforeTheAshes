#include "stdafx.h"
#include "Particle_Monster_Death.h"
#include "GameInstance.h"

CParticle_Monster_Death::CParticle_Monster_Death(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{
}

CParticle_Monster_Death::CParticle_Monster_Death(const CParticle_Monster_Death& Prototype)
    : CPartObject(Prototype)
{
}

HRESULT CParticle_Monster_Death::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle_Monster_Death::Initialize(void* pArg)
{
    PARTICLE_DEATH_DESC* pDesc = static_cast<PARTICLE_DEATH_DESC*>(pArg);
    m_pSocketMatrix = pDesc->pSocketMatrix;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    return S_OK;
}

_uint CParticle_Monster_Death::Priority_Update(_float fTimeDelta)
{
    return OBJ_NOEVENT;
}

void CParticle_Monster_Death::Update(_float fTimeDelta)
{
    if(true == m_bOn)
    {
        _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

        _vector vPos = SocketMatrix.r[3];
        SocketMatrix = XMMatrixIdentity();
        SocketMatrix.r[3] = vPos;
        // ??
        for (size_t i = 0; i < 3; i++)
        {
            SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
        }

        // 내 위치 * 붙여야 할 뼈의 위치 * 플레이어 위치 -> 플레이어의 위치에서 붙여야 할 뼈의 위치.
        // -> 셰이더에서 해주던 뼈 * 월드를 여기서 하는 거.
        XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

        _vector vCamLook = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW).r[2];
        vCamLook = XMVector3TransformNormal(vCamLook, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

        _vector vMove = XMVectorSet(0.f, 1.f, 0.f, 0.f) * fTimeDelta * 2.f;
        vMove = XMVector3TransformNormal(vMove, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
        XMStoreFloat3(&m_vPivot, XMLoadFloat3(&m_vPivot) - vMove);

        if (true == m_pVIBufferCom->Spread(XMLoadFloat3(&m_vPivot), vCamLook, 1.f, -0.01f, false, fTimeDelta))
            m_bDead = true;
    }
}

void CParticle_Monster_Death::Late_Update(_float fTimeDelta)
{
    if(true == m_bOn)
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticle_Monster_Death::Render()
{
    if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(3)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Monster_Death::Ready_Components(PARTICLE_DEATH_DESC* pDesc)
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Stone"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    CVIBuffer_Instancing::INSTANCE_DESC			ParticleDesc{};
    ZeroMemory(&ParticleDesc, sizeof ParticleDesc);
    ParticleDesc.iNumInstance = pDesc->iNumInstance;
    ParticleDesc.vCenter = pDesc->vCenter;
    ParticleDesc.vRange = pDesc->vRange;	// 이게 첫 생성 범위
    ParticleDesc.vExceptRange = pDesc->vExceptRange;
    ParticleDesc.vSize = pDesc->vSize;		// 이게 크기
    ParticleDesc.vSpeed = pDesc->vSpeed;
    ParticleDesc.vLifeTime = pDesc->vLifeTime;
    ParticleDesc.vMinColor = _float4(0.f, 0.f, 0.f, 1.f);
    ParticleDesc.vMaxColor = _float4(1.f, 1.f, 1.f, 1.f);

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &ParticleDesc)))
        return E_FAIL;

    return S_OK;
}

CParticle_Monster_Death* CParticle_Monster_Death::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Monster_Death* pInstance = new CParticle_Monster_Death(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CParticle_Monster_Death"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Monster_Death::Clone(void* pArg)
{
    CParticle_Monster_Death* pInstance = new CParticle_Monster_Death(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CParticle_Monster_Death"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Monster_Death::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
