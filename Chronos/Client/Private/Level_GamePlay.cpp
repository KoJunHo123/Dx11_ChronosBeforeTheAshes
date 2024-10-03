#include "stdafx.h"

#include "..\Public\Level_GamePlay.h"
#include "Camera_Container.h"
#include "GameInstance.h"

#include "Monster.h"
#include "Player.h"
#include "Terrain.h"
#include "Layer.h"
#include "PuzzleBase.h"
#include "Pedestal.h"
#include "WayPoint.h"
#include "Puzzle_Item.h"
#include "RuneKey.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize(_uint iLevelIndex)
{
	if (FAILED(__super::Initialize(iLevelIndex)))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Inventory()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Interaction()))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Paticle()))
	//	return E_FAIL;

	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Player"), TEXT("Coll_Monster"));
	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Monster"), TEXT("Coll_Monster"));
	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Player_Attack"), TEXT("Coll_Monster"));
	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Monster_Attack"), TEXT("Coll_Player"));
	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Player"), TEXT("Coll_Interaction"));
	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Player"), TEXT("Coll_Obstacle"));

	m_bLevelStart = true;

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	if (true == m_bLevelStart)
	{
		if (true == m_pGameInstance->FadeOut(fTimeDelta))
		{
			m_bLevelStart = false;
		}
	}

	if (true == m_pPlayer->Get_Dead())
	{
		m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));

		if (FAILED(Ready_Layer_Monster()))
			return;
	}
}

HRESULT CLevel_GamePlay::Render()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{	
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(-1.f, -5.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_Container"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

#ifndef _DEBUG
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Labyrinth"))))
		return E_FAIL;
#endif
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Inventory()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Inventory"), TEXT("Prototype_GameObject_Inventory"))))
		return E_FAIL;

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

	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0));
	Safe_AddRef(m_pPlayer);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster()
{
	// 맵툴에서 찍은 몬스터 가져올 것.
	// 시작 셀 인덱스만 받아오면 됨.

	CMonster::MONSTER_DESC desc = {};
	desc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.fSpeedPerSec = 1.f;
	
	desc.vRotation = {};
	desc.vScale = { 1.f, 1.f, 1.f };

	desc.iStartCellIndex = 150;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Boss_Lab"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 10;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Construct"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 20;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Drum"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 30;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Mage"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 40;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Troll"), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Paticle()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Snow"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Interaction()
{
	if (FAILED(Ready_Layer_Puzzle()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Teleport()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Pedestal()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Waypoint()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Puzzle()
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

HRESULT CLevel_GamePlay::Ready_Layer_Teleport()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Interaction"), TEXT("Prototype_GameObject_Teleport_Container"))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Pedestal()
{
	_char MaterialFilePath[MAX_PATH]{ "../Bin/SaveData/Pedestal.dat" };
	ifstream infile(MaterialFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;

	_uint iIndex = 0;
	while (true)
	{
		_float3 vPos = {};

		infile.read(reinterpret_cast<_char*>(&vPos), sizeof(_float3));

		if (true == infile.eof())
			break;

		CPedestal::PEDESTAL_DESC desc = {};
		desc.fRotationPerSec = 0.f;
		desc.fSpeedPerSec = 1.f;
		desc.vPos = vPos;
		
		
		if (0 == iIndex)
		{
			desc.strItemTag = TEXT("Item_ReplacePuzzle");
			desc.vRotation = { XMConvertToRadians(-45.f), XMConvertToRadians(180.f), 0.f };

			CPuzzle_Item::PUZZLEITEM_DESC ItemDesc = {};
			ItemDesc.fRotationPerSec = 0.f;
			ItemDesc.fSpeedPerSec = 1.f;

			desc.pItem = static_cast<CItem*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_PuzzleItem"), &desc));
		}
		else if (1 == iIndex)
		{
			desc.strItemTag = TEXT("Item_RuneKey");
			desc.vRotation = { XMConvertToRadians(45.f), XMConvertToRadians(180.f), 0.f };

			CRuneKey::RUNEKYE_DESC ItemDesc = {};
			ItemDesc.fRotationPerSec = 0.f;
			ItemDesc.fSpeedPerSec = 1.f;

			desc.pItem = static_cast<CItem*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_RuneKey"), &desc));
		}

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Interaction"), TEXT("Prototype_GameObject_Pedestal"), &desc)))
			return E_FAIL;

		++iIndex;
	}

	infile.close();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Waypoint()
{
	CWayPoint::WAYPOINT_DESC desc;
	desc.fRotationPerSec = 0.;
	desc.fSpeedPerSec = 1.f;
	desc.vPos = _float3(66.5f, 5.f, 101.5f);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Interaction"), TEXT("Prototype_GameObject_WayPoint"), &desc)))
		return E_FAIL;

	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelIndex)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iLevelIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

	Safe_Release(m_pPlayer);
}
