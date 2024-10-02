#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"

#include "GameInstance.h"

#include "BackGround.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(_uint iLevelIndex, LEVELID eNextLevelID)
{
	if (FAILED(__super::Initialize(iLevelIndex)))
		return E_FAIL;

	m_eNextLevelID = eNextLevelID;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;	
		

	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	m_bLevelStart = true;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	if ((GetKeyState(VK_SPACE) & 0x8000) &&
		true == m_pLoader->isFinished())
	{
		m_bLevelOver = true;
	}	

	if(true == m_bLevelOver)
	{
		if(true == m_pGameInstance->FadeIn(fTimeDelta))
		{
			Change_Level();
			m_bLevelOver = false;
		}
	}

	if (true == m_bLevelStart)
	{
		if (true == m_pGameInstance->FadeOut(fTimeDelta))
		{
			m_bLevelStart = false;
		}
	}

}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Draw_LoadingText();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround()
{
	CBackGround::UI_BACKGROUND_DESC BackGroundDesc = {};
	BackGroundDesc.eLevelID = LEVEL_LOADING;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOADING, TEXT("Layer_BackGround"),
		TEXT("Prototype_GameObject_BackGround"), &BackGroundDesc)))
		return E_FAIL;

	CUIObject::UI_DESC UIDesc = {};

	UIDesc.fX = g_iWinSizeX >> 1;
	UIDesc.fY = 300.f;
	UIDesc.fSizeX = 512.f;
	UIDesc.fSizeY = 512.f;
	UIDesc.fSpeedPerSec = 10.f;
	UIDesc.fRotationPerSec = XMConvertToRadians(90.0f);


	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOADING, TEXT("Layer_UI"),
		TEXT("Prototype_GameObject_UI_LoadingMaze"), &UIDesc)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Change_Level()
{
	CLevel* pNewLevel = { nullptr };

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext, LEVEL_LOGO);
		break;
	case LEVEL_GAMEPLAY:
		pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY);
		break;
	}

	if (nullptr == pNewLevel)
		return;

	m_pGameInstance->Change_Level(pNewLevel);


}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelIndex, LEVELID eNextLevelID)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iLevelIndex, eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
