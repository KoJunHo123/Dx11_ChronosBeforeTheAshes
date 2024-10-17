#include "stdafx.h"
#include "Player_UseSkill_Particle.h"
#include "GameInstance.h"

#include "Player.h"

CPlayer_UseSkill_Particle::CPlayer_UseSkill_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CPlayer_UseSkill_Particle::CPlayer_UseSkill_Particle(const CPartObject& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CPlayer_UseSkill_Particle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_UseSkill_Particle::Initialize(void* pArg)
{
	PLAYER_USESKILL_DESC* pDesc = static_cast<PLAYER_USESKILL_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
	m_pCurrentSkill = pDesc->pCurrentSkill;
	m_pSkilDuration = pDesc->pSkilDuration;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	

	return S_OK;
}

_uint CPlayer_UseSkill_Particle::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CPlayer_UseSkill_Particle::Update(_float fTimeDelta)
{
	if (true == m_bOn)
	{
		_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
		for (size_t i = 0; i < 3; i++)
		{
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
		}

		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

		if (CPlayer::SKILL_RED == *m_pCurrentSkill)
			m_vDivide = _float2(5.f, 5.f);
		else
			m_vDivide = _float2(7.f, 7.f);

		if (true == m_pVIBufferCom->Trail_Spread(XMLoadFloat4x4(&m_WorldMatrix), XMVectorSet(0.f, 0.f, 0.f, 1.f), -2.f, false, fTimeDelta))
		{
			m_pVIBufferCom->Reset();
			m_bOn = false;
		}
	}
}

void CPlayer_UseSkill_Particle::Late_Update(_float fTimeDelta)
{
	if(true == m_bOn)
	{
		if (CPlayer::SKILL_RED == *m_pCurrentSkill)
		{
			m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this);
		}
		else
		{
			m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
		}
	}
}

HRESULT CPlayer_UseSkill_Particle::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexDivide", &m_vDivide, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iSkillIndex", m_pCurrentSkill, sizeof(_uint))))
		return E_FAIL;

	if (CPlayer::SKILL_RED == *m_pCurrentSkill)
	{
		if (FAILED(m_pFireTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 3)))
			return E_FAIL;

		if (FAILED(m_pFireTextureCom->Bind_ShadeResource(m_pShaderCom, "g_SubTexture", 5)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(6)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShadowTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

		if (FAILED(m_pShadowTextureCom->Bind_ShadeResource(m_pShaderCom, "g_SubTexture", 1)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(11)))
			return E_FAIL;
	}


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_UseSkill_Particle::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleSkill_Fire"),
		TEXT("Com_Texture_Fire"), reinterpret_cast<CComponent**>(&m_pFireTextureCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ParticleSkill_Shadow"),
		TEXT("Com_Texture_Shadow"), reinterpret_cast<CComponent**>(&m_pShadowTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_UseSkill"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CPlayer_UseSkill_Particle* CPlayer_UseSkill_Particle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_UseSkill_Particle* pInstance = new CPlayer_UseSkill_Particle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_UseSkill_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_UseSkill_Particle::Clone(void* pArg)
{
	CPlayer_UseSkill_Particle* pInstance = new CPlayer_UseSkill_Particle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CPlayer_UseSkill_Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_UseSkill_Particle::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pFireTextureCom);
	Safe_Release(m_pShadowTextureCom);
}
