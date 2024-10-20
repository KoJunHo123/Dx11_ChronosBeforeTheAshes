#include "stdafx.h"
#include "..\Public\UI_TextBox.h"

#include "GameInstance.h"

#include "Level_Loading.h"

CUI_TextBox::CUI_TextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlend_UI_Object{ pDevice, pContext }
{
}

CUI_TextBox::CUI_TextBox(const CUI_TextBox& Prototype)
	: CBlend_UI_Object{ Prototype }
{
}

HRESULT CUI_TextBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_TextBox::Initialize(void* pArg)
{
	UI_TEXTBOX_DESC* pDesc = static_cast<UI_TEXTBOX_DESC*>(pArg);
	m_strText = pDesc->strText;
	m_fOffsetX = pDesc->fOffsetX;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fRatio = 0.5f;

	return S_OK;
}

_uint CUI_TextBox::Priority_Update(_float fTimeDelta)
{
	GetCursorPos(&m_ptMouse);
	ScreenToClient(g_hWnd, &m_ptMouse);

	return OBJ_NOEVENT;
}

void CUI_TextBox::Update(_float fTimeDelta)
{
	if(true == On_MousePoint(m_ptMouse) && m_pGameInstance->Get_DIMouseState(DIMK_LBUTTON))
	{
		if (TEXT("게임 시작") == m_strText)
		{
			m_bLevelChange = true;
			m_pGameInstance->StopSoundSlowly(SOUND_BGM);
		}
		else if (TEXT("나가기") == m_strText)
		{
			_int iExitCode = 0;
			PostQuitMessage(iExitCode);
		}
	}

	if (true == m_bLevelChange && true == m_pGameInstance->FadeIn(fTimeDelta))
	{
		m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOADING, LEVEL_GAMEPLAY));
	}

}

void CUI_TextBox::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLENDUI, this);
}

HRESULT CUI_TextBox::Render()
{
	if (FAILED(m_pGameInstance->Render_Text(TEXT("Font_145"), m_strText.c_str(), XMVectorSet(m_fX, m_fY, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.6f, true)))
		return E_FAIL;

	if(true == On_MousePoint(m_ptMouse))
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
	}

	return S_OK;
}

_bool CUI_TextBox::On_MousePoint(POINT ptMouse)
{
	if (m_fX - (m_fSizeX * 0.5f) < ptMouse.x && ptMouse.x < m_fX + (m_fSizeX * 0.5f)
		&& m_fY - (m_fSizeY * 0.25f) < ptMouse.y && ptMouse.y < m_fY + (m_fSizeY * 0.25f))
		return true;

	return false;
}

HRESULT CUI_TextBox::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_TextBox"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_TextBox* CUI_TextBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_TextBox* pInstance = new CUI_TextBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_TextBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_TextBox::Clone(void* pArg)
{
	CUI_TextBox* pInstance = new CUI_TextBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_TextBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_TextBox::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
