#include "stdafx.h"
#include "..\Public\UI_LogoMaze.h"

#include "GameInstance.h"

CUI_LogoMaze::CUI_LogoMaze(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlend_UI_Object{ pDevice, pContext }
{
}

CUI_LogoMaze::CUI_LogoMaze(const CUI_LogoMaze& Prototype)
	: CBlend_UI_Object{ Prototype }
{
}

HRESULT CUI_LogoMaze::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_LogoMaze::Initialize(void* pArg)
{
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fRatio = .3f;

	return S_OK;
}

_uint CUI_LogoMaze::Priority_Update(_float fTimeDelta)
{

	return OBJ_NOEVENT;
}

void CUI_LogoMaze::Update(_float fTimeDelta)
{
	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta * -0.05f);
}

void CUI_LogoMaze::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLENDUI, this);
}

HRESULT CUI_LogoMaze::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &m_fRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(5)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_LogoMaze::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Logo_Maze"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_LogoMaze* CUI_LogoMaze::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_LogoMaze* pInstance = new CUI_LogoMaze(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_LogoMaze"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_LogoMaze::Clone(void* pArg)
{
	CUI_LogoMaze* pInstance = new CUI_LogoMaze(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_LogoMaze"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_LogoMaze::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
