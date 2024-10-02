#include "stdafx.h"
#include "..\Public\UI_GlowStar.h"

#include "GameInstance.h"

CUI_GlowStar::CUI_GlowStar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlend_UI_Object{ pDevice, pContext }
{
}

CUI_GlowStar::CUI_GlowStar(const CUI_GlowStar& Prototype)
	: CBlend_UI_Object{ Prototype }
{
}

HRESULT CUI_GlowStar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_GlowStar::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fRatio = 1.f;

	return S_OK;
}

_uint CUI_GlowStar::Priority_Update(_float fTimeDelta)
{

	return OBJ_NOEVENT;
}

void CUI_GlowStar::Update(_float fTimeDelta)
{
	if (m_fRatio <= 0.5f)
		m_bAlphaControl = true;
	else if(m_fRatio >= 1.f)
		m_bAlphaControl = false;

	if (true == m_bAlphaControl)
		m_fRatio += fTimeDelta * 0.2f;
	else
		m_fRatio -= fTimeDelta * 0.2f;
}

void CUI_GlowStar::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLENDUI, this);
}

HRESULT CUI_GlowStar::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &m_fRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(5)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_GlowStar::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_Glow_Star"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_GlowStar* CUI_GlowStar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_GlowStar* pInstance = new CUI_GlowStar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_GlowStar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_GlowStar::Clone(void* pArg)
{
	CUI_GlowStar* pInstance = new CUI_GlowStar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_GlowStar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_GlowStar::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
