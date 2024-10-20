#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "Level_Loading.h"

#include "GameInstance.h"

#include "BackGround.h"
#include "UI_TextBox.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize(_uint iLevelIndex)
{
	if (FAILED(__super::Initialize(iLevelIndex)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_145"), TEXT("../Bin/Resources/Fonts/143ex.spritefont"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	m_bLevelStart = true;

	m_pGameInstance->PlayBGM(TEXT("Mus_Chronos_Theme_All.ogg"), SOUND_BGM, 1.f);

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if (true == m_bLevelStart)
	{
		if (true == m_pGameInstance->FadeOut(fTimeDelta))
		{
			m_bLevelStart = false;
		}
	}
}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	CBackGround::UI_BACKGROUND_DESC desc = {};
	desc.eLevelID = LEVEL_LOGO;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, strLayerTag,TEXT("Prototype_GameObject_BackGround"), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(const _wstring& strLayerTag)
{
	CUIObject::UI_DESC UIDesc = {};

	//UIDesc.fX = g_iWinSizeX * 0.5f + 5.f;
	UIDesc.fX = g_iWinSizeX >> 1;
	UIDesc.fX += 5.f;
	UIDesc.fY = 205.f;
	UIDesc.fSizeX = 970.f;
	UIDesc.fSizeY = 675.f;
	UIDesc.fSpeedPerSec = 10.f;
	UIDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, strLayerTag, TEXT("Prototype_GameObject_UI_GlowStar"), &UIDesc)))
		return E_FAIL;

	UIDesc.fSizeX = 512.f;
	UIDesc.fSizeY = 512.f;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, strLayerTag, TEXT("Prototype_GameObject_UI_LogoMaze"), &UIDesc)))
		return E_FAIL;

	UIDesc.fX = g_iWinSizeX >> 1;
	UIDesc.fY = 200.f;
	UIDesc.fSizeX = 1024.f;
	UIDesc.fSizeY = 512.f;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, strLayerTag, TEXT("Prototype_GameObject_UI_LogoTitle"), &UIDesc)))
		return E_FAIL;

	CUI_TextBox::UI_TEXTBOX_DESC TextBoxDesc = {};
	TextBoxDesc.fRotationPerSec = UIDesc.fRotationPerSec;
	TextBoxDesc.fSpeedPerSec = UIDesc.fSpeedPerSec;
	TextBoxDesc.fX = UIDesc.fX;
	TextBoxDesc.fY = 500.f;
	//TextBoxDesc.fSizeX = 231.f;
	//TextBoxDesc.fSizeY = 192.f;
	TextBoxDesc.fSizeX = 173.25;
	TextBoxDesc.fSizeY = 144.f;
	TextBoxDesc.fOffsetX = -13.f;
	TextBoxDesc.strText = TEXT("게임 시작");

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, strLayerTag, TEXT("Prototype_GameObject_UI_TextBox"), &TextBoxDesc)))
		return E_FAIL;

	TextBoxDesc.fY += 100.f;
	TextBoxDesc.strText = TEXT("나가기");
	TextBoxDesc.fOffsetX = 0.f;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, strLayerTag, TEXT("Prototype_GameObject_UI_TextBox"), &TextBoxDesc)))
		return E_FAIL;
	
	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelIndex)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iLevelIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Logo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();

}
