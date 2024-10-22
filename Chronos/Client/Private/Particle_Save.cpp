#include "stdafx.h"
#include "Particle_Save.h"
#include "GameInstance.h"

CParticle_Save::CParticle_Save(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}
CParticle_Save::CParticle_Save(const CParticle_Save& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CParticle_Save::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Save::Initialize(void* pArg)
{
	PARTICLE_SAVE_DESC* pDesc = static_cast<PARTICLE_SAVE_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_Position(XMLoadFloat3(&pDesc->vPos));

	return S_OK;
}

_uint CParticle_Save::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CParticle_Save::Update(_float fTimeDelta)
{
	if (true == m_pVIBufferCom->Spread_Random(XMVectorSet(0.f, 4.f, 0.f, 1.f), 1.f, 1.f, false, 1.f, fTimeDelta))
		m_bDead = true;
}

void CParticle_Save::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
}

HRESULT CParticle_Save::Render()
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
	if (FAILED(m_pShaderCom->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Save::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Glow"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Save"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CParticle_Save* CParticle_Save::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Save* pInstance = new CParticle_Save(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CParticle_Save"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticle_Save::Clone(void* pArg)
{
	CParticle_Save* pInstance = new CParticle_Save(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CParticle_Save"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticle_Save::Pooling()
{
	return new CParticle_Save(*this);
}

void CParticle_Save::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
