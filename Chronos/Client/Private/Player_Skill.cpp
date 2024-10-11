#include "stdafx.h"
#include "Player_Skill.h"
#include "GameInstance.h"

#include "Player.h"

CPlayer_Skill::CPlayer_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlend_PartObject(pDevice, pContext)
{
}

CPlayer_Skill::CPlayer_Skill(const CPlayer_Skill& Prototype)
	: CBlend_PartObject(Prototype)
{
}

HRESULT CPlayer_Skill::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Skill::Initialize(void* pArg)
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

	m_vDivide = _float2(9.f, 9.f);
	
	return S_OK;
}

_uint CPlayer_Skill::Priority_Update(_float fTimeDelta)
{

	XMStoreFloat3(&m_vPrePosition, XMLoadFloat4x4(&m_WorldMatrix).r[3]);

	return OBJ_NOEVENT;
}

void CPlayer_Skill::Update(_float fTimeDelta)
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

		_matrix TailWorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * TailSocketMatrix * XMLoadFloat4x4(m_pParentMatrix);
		XMStoreFloat3(&m_vTailPos, TailWorldMatrix.r[3]);

		m_fTexIndex += fTimeDelta * 30.f;
		if (m_fTexIndex > m_vDivide.x * m_vDivide.y - 1.f)
			m_fTexIndex = 0.f;
	}
	else
		m_fTexIndex = 0.f;

}

void CPlayer_Skill::Late_Update(_float fTimeDelta)
{
	Compute_ViewZ();

	if (0.f < *m_pSkilDuration)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CPlayer_Skill::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_int iTexIndex = (_int)m_fTexIndex;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_TexDivide", &m_vDivide, sizeof _float2)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iTexIndex", &iTexIndex, sizeof _int)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iSkillIndex", m_pCurrentSkill, sizeof _uint)))
		return E_FAIL;

	if (FAILED(m_pLightTextureCom->Bind_ShadeResource(m_pShaderCom, "g_LightTexture", 0)))
		return E_FAIL;

	if (CPlayer::SKILL_RED == *m_pCurrentSkill)
	{
		if (FAILED(m_pFireTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShadowTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(0)))
		return E_FAIL;

	return S_OK;
}



HRESULT CPlayer_Skill::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Skill"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_FireTexture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PlayerSkill_Fire"),
		TEXT("Com_FireTexture"), reinterpret_cast<CComponent**>(&m_pFireTextureCom))))
		return E_FAIL;

	/* FOR.Com_ShadowTexture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PlayerSkill_Shadow"),
		TEXT("Com_ShadowTexture"), reinterpret_cast<CComponent**>(&m_pShadowTextureCom))))
		return E_FAIL;


	/* FOR.Com_LightTexture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PlayerSkill_SwordLight"),
		TEXT("Com_LightTexture"), reinterpret_cast<CComponent**>(&m_pLightTextureCom))))
		return E_FAIL;
	
	return S_OK;
}



CPlayer_Skill* CPlayer_Skill::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Skill* pInstance = new CPlayer_Skill(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_Skill"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Skill::Clone(void* pArg)
{
	CPlayer_Skill* pInstance = new CPlayer_Skill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CPlayer_Skill"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Skill::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFireTextureCom);
	Safe_Release(m_pShadowTextureCom);
	Safe_Release(m_pLightTextureCom);
}
