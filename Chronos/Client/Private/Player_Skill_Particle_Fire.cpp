#include "stdafx.h"
#include "Player_Skill_Particle_Fire.h"
#include "GameInstance.h"

#include "Player.h"

CPlayer_Skill_Particle_Fire::CPlayer_Skill_Particle_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{
}

CPlayer_Skill_Particle_Fire::CPlayer_Skill_Particle_Fire(const CPartObject& Prototype)
    : CPartObject(Prototype)
{
}

HRESULT CPlayer_Skill_Particle_Fire::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer_Skill_Particle_Fire::Initialize(void* pArg)
{
    PLAYER_SKILL_DESC* pDesc = static_cast<PLAYER_SKILL_DESC*>(pArg);

    m_pSocketMatrix = pDesc->pSocketBoneMatrix;
    m_pTailSocketMatrix = pDesc->pTailSocketMatrix;
    m_pCurrentSkill = pDesc->pCurrentSkill;
    m_pSkilDuration = pDesc->pSkilDuration;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_vColor = _float4(0.541f, 0.169f, 0.886f, 1.f);
    //m_vColor = _float4(0.749f, 0.251f, 1.0f, 1.f);

    return S_OK;
}

_uint CPlayer_Skill_Particle_Fire::Priority_Update(_float fTimeDelta)
{

    return OBJ_NOEVENT;
}

void CPlayer_Skill_Particle_Fire::Update(_float fTimeDelta)
{
    if (0.f < *m_pSkilDuration)
    {
        _matrix TailSocketMatrix = XMLoadFloat4x4(m_pTailSocketMatrix);
        _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

        _float3 vScale = _float3(XMVectorGetX(XMVector3Length(SocketMatrix.r[0])),
            XMVectorGetX(XMVector3Length(SocketMatrix.r[1])),
            XMVectorGetX(XMVector3Length(SocketMatrix.r[2])));

        _vector vPosition = SocketMatrix.r[3];

        _vector		vLook = TailSocketMatrix.r[3] - vPosition;
        _vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
        _vector		vUp = XMVector3Cross(vLook, vRight);

        XMStoreFloat3((_float3*)&(*m_pSocketMatrix).m[0][0], XMVector3Normalize(vRight) * vScale.x);
        XMStoreFloat3((_float3*)&(*m_pSocketMatrix).m[1][0], XMVector3Normalize(vUp) * vScale.y);
        XMStoreFloat3((_float3*)&(*m_pSocketMatrix).m[2][0], XMVector3Normalize(vLook) * vScale.z);

        SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

        // ??
        for (size_t i = 0; i < 3; i++)
        {
            SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
        }

        // 내 위치 * 붙여야 할 뼈의 위치 * 플레이어 위치 -> 플레이어의 위치에서 붙여야 할 뼈의 위치.
        // -> 셰이더에서 해주던 뼈 * 월드를 여기서 하는 거.
        XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

        m_vDivide = _float2(5.f, 5.f);
      
        if(CPlayer::SKILL_RED == *m_pCurrentSkill)
            m_pRedVIBufferCom->Trail_Spread(XMLoadFloat4x4(&m_WorldMatrix), XMVectorSet(0.f, 0.f, 0.f, 0.f), -1.f, true, fTimeDelta);
        else
            m_pPuppleVIBufferCom->Trail_Spread(XMLoadFloat4x4(&m_WorldMatrix), XMVectorSet(0.f, 0.f, 0.f, 0.f), -1.f, true, fTimeDelta);

        m_bReset = false;
    }
    else
    {
        if (false == m_bReset)
        {
            m_vDivide = _float2(5.f, 5.f);
            if (CPlayer::SKILL_RED == *m_pCurrentSkill)
            {
                if (true == m_pRedVIBufferCom->Trail_Spread(XMLoadFloat4x4(&m_WorldMatrix), XMVectorSet(0.f, 0.f, 0.f, 0.f), -1.f, false, fTimeDelta))
                {
                    m_pRedVIBufferCom->Reset();
                    m_bReset = true;
                }
            }
            else
            {
                if (true == m_pPuppleVIBufferCom->Trail_Spread(XMLoadFloat4x4(&m_WorldMatrix), XMVectorSet(0.f, 0.f, 0.f, 0.f), -1.f, false, fTimeDelta))
                {
                    m_pPuppleVIBufferCom->Reset();
                    m_bReset = true;
                }
            }
        }
    }
}

void CPlayer_Skill_Particle_Fire::Late_Update(_float fTimeDelta)
{
    if (false == m_bReset)
    {
        m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
    }
}

HRESULT CPlayer_Skill_Particle_Fire::Render()
{
    // 월드 바인딩 x
    // 셰이더 만들고 테스트.
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexDivide", &m_vDivide, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 3)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_SubTexture", 5)))
        return E_FAIL;

    if(CPlayer::SKILL_RED == *m_pCurrentSkill)
    {
        if (FAILED(m_pShaderCom->Begin(6)))
            return E_FAIL;

        if (FAILED(m_pRedVIBufferCom->Bind_Buffers()))
            return E_FAIL;
        if (FAILED(m_pRedVIBufferCom->Render()))
            return E_FAIL;
    }
    else
    {
        if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
            return E_FAIL;
        if (FAILED(m_pShaderCom->Begin(10)))
            return E_FAIL;

        if (FAILED(m_pPuppleVIBufferCom->Bind_Buffers()))
            return E_FAIL;
        if (FAILED(m_pPuppleVIBufferCom->Render()))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CPlayer_Skill_Particle_Fire::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleSkill_Fire"),
        TEXT("Com_Texture_Fire"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_RedVIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Skill_Red"),
        TEXT("Com_RedVIBuffer"), reinterpret_cast<CComponent**>(&m_pRedVIBufferCom))))
        return E_FAIL;

    /* FOR.Com_PuppleVIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Skill_Pupple"),
        TEXT("Com_PuppleVIBuffer"), reinterpret_cast<CComponent**>(&m_pPuppleVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CPlayer_Skill_Particle_Fire* CPlayer_Skill_Particle_Fire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer_Skill_Particle_Fire* pInstance = new CPlayer_Skill_Particle_Fire(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CPlayer_Skill_Particle_Fire"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer_Skill_Particle_Fire::Clone(void* pArg)
{
    CPlayer_Skill_Particle_Fire* pInstance = new CPlayer_Skill_Particle_Fire(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CPlayer_Skill_Particle_Fire"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer_Skill_Particle_Fire::Pooling()
{
    return new CPlayer_Skill_Particle_Fire(*this);
}

void CPlayer_Skill_Particle_Fire::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pRedVIBufferCom);
    Safe_Release(m_pPuppleVIBufferCom);
}
