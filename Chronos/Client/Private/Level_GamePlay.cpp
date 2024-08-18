#include "stdafx.h"

#include "..\Public\Level_GamePlay.h"
#include "FreeCamera.h"
#include "GameInstance.h"

#include "Monster.h"
#include "Player.h"
#include "Terrain.h"
#include "Layer.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Load_Player()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;


	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	
}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera()
{
	CFreeCamera::CAMERA_FREE_DESC		Desc{};

	Desc.fSensor = 0.2f;
	Desc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround()
{
	CTerrain::TERRAIN_DESC desc{};
	desc.vPos = XMVectorSet(-168.f, 4.725f, 157.f, 0.f);
	desc.fRotationPerSec = 0.f;
	desc.fSpeedPerSec = 0.f;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Terrain"), &desc)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Labyrinth"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player()
{
	CPlayer::PLAYER_DESC desc = {};

	desc.fRotationPerSec = 90.f;
	desc.fSpeedPerSec = 1.f;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster()
{
	CMonster::MONSTER_DESC desc = {};

	desc.fRotationPerSec = 0.f;
	desc.fSpeedPerSec = 0.f;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster"), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Player()
{
	m_pGameInstance->Create_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	_char MaterialFilePath[MAX_PATH]{ "../Bin/SaveData/Player.dat" };
	ifstream infile(MaterialFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;
	while (true)
	{
		if (FAILED(Ready_Layer_Player()))
			return E_FAIL;

		if(FAILED(m_pGameInstance->Load_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), &infile)))
			break;
	}

	infile.close();

	return S_OK;
}

// 클라
HRESULT CLevel_GamePlay::Load_Monsters()
{
	m_pGameInstance->Create_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));

	_char MaterialFilePath[MAX_PATH]{ "../Bin/SaveData/Monster.dat" };
	ifstream infile(MaterialFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;
	while(true)
	{
		if (FAILED(Ready_Layer_Monster()))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Load_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &infile)))
			break;
	}

	infile.close();

	return S_OK;
}


CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}
