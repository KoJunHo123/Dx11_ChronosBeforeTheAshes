#include "stdafx.h"
#include "Player_Effect.h"
#include "GameInstance.h"

CPlayer_Effect::CPlayer_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CPlayer_Effect::CPlayer_Effect(const CPlayer_Effect& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CPlayer_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Effect::Initialize(void* pArg)
{
	EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_Position(Get_Position() + XMVectorSet(0.f, 10.f, 0.f, 0.f));

	m_vDivide = _float2(8, 8);

	return S_OK;
}

_uint CPlayer_Effect::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CPlayer_Effect::Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

	m_pVIBufferCom->Trail_Points(XMLoadFloat4x4(&m_WorldMatrix), XMVectorSet(0.f, 1.f, 0.f, 0.f), true, fTimeDelta);
}

void CPlayer_Effect::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CPlayer_Effect::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexDivide", &m_vDivide, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(12)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Effect::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FloatyBits"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_FloatyBits"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CPlayer_Effect* CPlayer_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Effect* pInstance = new CPlayer_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Effect::Clone(void* pArg)
{
	CPlayer_Effect* pInstance = new CPlayer_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CPlayer_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Effect::Pooling()
{
	return new CPlayer_Effect(*this);
}

void CPlayer_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
