#include "stdafx.h"
#include "Lab_Construct_Effect_Pupple.h"
#include "GameInstance.h"

CLab_Construct_Effect_Pupple::CLab_Construct_Effect_Pupple(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{
}

CLab_Construct_Effect_Pupple::CLab_Construct_Effect_Pupple(const CLab_Construct_Effect_Pupple& Prototype)
    : CPartObject(Prototype)
{
}

HRESULT CLab_Construct_Effect_Pupple::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLab_Construct_Effect_Pupple::Initialize(void* pArg)
{
    EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

    m_pSocketMatrix = pDesc->pSocketMatrix;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_vDivide = _float2(5.f, 5.f);
    m_vColor = _float4(0.541f, 0.169f, 0.886f, 1.f);

    return S_OK;
}

_uint CLab_Construct_Effect_Pupple::Priority_Update(_float fTimeDelta)
{
    return OBJ_NOEVENT;
}

void CLab_Construct_Effect_Pupple::Update(_float fTimeDelta)
{
    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    // ??
    for (size_t i = 0; i < 3; i++)
    {
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
    }

    // 내 위치 * 붙여야 할 뼈의 위치 * 플레이어 위치 -> 플레이어의 위치에서 붙여야 할 뼈의 위치.
    // -> 셰이더에서 해주던 뼈 * 월드를 여기서 하는 거.
    XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
    if (false == m_bOver)
    {
        m_pVIBufferCom->Trail_Spread(XMLoadFloat4x4(&m_WorldMatrix), XMVectorSet(0.f, 0.f, 0.f, 1.f), -1.f, m_bOn, fTimeDelta);
        if (true == m_bOver)
        {
            m_pVIBufferCom->Reset();
        }
    }
}

void CLab_Construct_Effect_Pupple::Late_Update(_float fTimeDelta)
{
    if(false == m_bOver)
        m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}

HRESULT CLab_Construct_Effect_Pupple::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 3)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexDivide", &m_vDivide, sizeof(_float2))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(10)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLab_Construct_Effect_Pupple::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleSkill_Fire"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Construct_Pupple"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CLab_Construct_Effect_Pupple* CLab_Construct_Effect_Pupple::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLab_Construct_Effect_Pupple* pInstance = new CLab_Construct_Effect_Pupple(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CLab_Construct_Effect_Pupple"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CLab_Construct_Effect_Pupple* CLab_Construct_Effect_Pupple::Clone(void* pArg)
{
    CLab_Construct_Effect_Pupple* pInstance = new CLab_Construct_Effect_Pupple(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CLab_Construct_Effect_Pupple"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLab_Construct_Effect_Pupple::Pooling()
{
    return new CLab_Construct_Effect_Pupple(*this);
}

void CLab_Construct_Effect_Pupple::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
}
