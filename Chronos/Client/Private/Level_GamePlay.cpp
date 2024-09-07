#include "stdafx.h"

#include "..\Public\Level_GamePlay.h"
#include "FreeCamera.h"
#include "GameInstance.h"

#include "Monster.h"
#include "Player.h"
#include "Terrain.h"
#include "Layer.h"
#include "PuzzleBase.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;
	

	if (FAILED(Ready_Layer_Interaction()))
		return E_FAIL;
	
	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Player"), TEXT("Coll_Monster"));
	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Monster"), TEXT("Coll_Monster"));
	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Player_Attack"), TEXT("Coll_Monster"));
	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Monster_Attack"), TEXT("Coll_Player"));


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

	Desc.fSensor = 0.1f;
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
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Labyrinth"))))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player()
{
	_char MaterialFilePath[MAX_PATH]{ "../Bin/SaveData/Player.dat" };
	ifstream infile(MaterialFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;
	while (true)
	{
		CPlayer::PLAYER_DESC desc{};

		infile.read(reinterpret_cast<_char*>(&desc), sizeof(CPlayer::PLAYER_DESC));

		if (true == infile.eof())
			break;

		desc.fRotationPerSec = XMConvertToRadians(90.0f);

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &desc)))
			return E_FAIL;

	}

	infile.close();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster()
{
	_char MaterialFilePath[MAX_PATH]{ "../Bin/SaveData/Boss_Lab.dat" };
	ifstream infile(MaterialFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;
	while (true)
	{
		CMonster::MONSTER_DESC desc{};

		infile.read(reinterpret_cast<_char*>(&desc), sizeof(CMonster::MONSTER_DESC));

		if (true == infile.eof())
			break;

		desc.fRotationPerSec = XMConvertToRadians(90.0f);

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Boss_Lab"), &desc)))
			return E_FAIL;
	}

	infile.close();

	
	CMonster::MONSTER_DESC desc = {};
	desc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.fSpeedPerSec = 1.f;
	
	desc.vPos = {};
	desc.vRotation = {};
	desc.vScale = { 1.f, 1.f, 1.f };

	desc.iStartCellIndex = 10;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Construct"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 14;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Drum"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 18;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Mage"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 22;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Troll"), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Interaction()
{
	_char MaterialFilePath[MAX_PATH]{ "../Bin/SaveData/PuzzleBase.dat" };
	ifstream infile(MaterialFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;

	while (true)
	{
		CPuzzleBase::PUZZLEBASE_DESC desc = {};

		infile.read(reinterpret_cast<_char*>(&desc), sizeof(CPuzzleBase::PUZZLEBASE_DESC));

		if (true == infile.eof())
			break;

		desc.fRotationPerSec = XMConvertToRadians(90.0f);
		desc.fSpeedPerSec = 1.f;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Interaction"), TEXT("Prototype_GameObject_PuzzleBase"), &desc)))
			return E_FAIL;
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
