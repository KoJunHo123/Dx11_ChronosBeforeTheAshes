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
#include "DoorLock.h"
#include "Inventory.h"

#include "UI_PlayerBase.h"


#include "Effect_Distortion.h"


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

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Interaction()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;

	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Player"), TEXT("Coll_Monster"));
	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Monster"), TEXT("Coll_Monster"));
	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Player_Attack"), TEXT("Coll_Monster"));
	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Monster_Attack"), TEXT("Coll_Player"));
	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Player"), TEXT("Coll_Interaction"));
	m_pGameInstance->Add_CollisionKeys(TEXT("Coll_Player"), TEXT("Coll_Obstacle"));

	m_bLevelStart = true;

	//if(FAILED(Ready_Layer_Monster()))
	//	return E_FAIL;

	m_pGameInstance->StopSound(SOUND_BGM);
	m_pGameInstance->PlayBGM(TEXT("Amb_Labyrinth_Dark_Exterior_Loop.ogg"), SOUND_BGM, 0.5f);

	 SOUND_DESC desc = {};
	 desc.fVolume = 1.f;

	 m_pGameInstance->SoundPlay_Additional(TEXT("Mus_Zone_Theme_Labyrinth.ogg"), desc);

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta;

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
		m_pGameInstance->StopAll();

		SOUND_DESC desc = {};
		desc.fVolume = 1.f;
		m_pGameInstance->SoundPlay_Additional(TEXT("Mus_Zone_Theme_Labyrinth.ogg"), desc);


		m_bYellow = false;
		m_bPupple = false;
		m_bRed = false;
	}

	_uint iCellIndex = m_pPlayer->Get_CellIndex();

	// 노랑
	if (false == m_bYellow && (6049 == iCellIndex || 6050 == iCellIndex || 5427 == iCellIndex))
	{
		Ready_Layer_Monster_Yellow();
		m_bYellow = true;
	}
	// 보라
	else if (false == m_bPupple && ((6084 <= iCellIndex && iCellIndex <= 6087) || 6129 == iCellIndex))
	{
		Ready_Layer_Monster_Pupple();
		m_bPupple = true;
	}
	// 빨강
	else if (false == m_bRed && (6082 == iCellIndex || 6083 == iCellIndex || 4689 == iCellIndex))
	{
		Ready_Layer_Monster_Red();
		m_bRed = true;
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
	LightDesc.vDirection = _float4(-1.f, -1.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.fRange = 0.f;
	LightDesc.vDiffuse = _float4(1.0f, 0.271f, 0.0f, 1.f);
	LightDesc.vAmbient = /*_float4(0.4f, 0.2f, 0.2f, 1.f);*/_float4(0.f, 0.f, 0.f, 0.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

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
	CGameObject::GAMEOBJECT_DESC desc = {};
	desc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.fSpeedPerSec = 1.f;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Inventory"), TEXT("Prototype_GameObject_Inventory"), &desc)))
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
	CMonster::MONSTER_DESC desc = {};
	desc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.fSpeedPerSec = 1.f;

	desc.vRotation = {};
	desc.vScale = { 1.f, 1.f, 1.f };
	desc.iStartCellIndex = 50;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Mage"), &desc)))
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

	if (FAILED(Ready_Layer_DoorLock()))
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
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_TeleportContainer"), TEXT("Prototype_GameObject_Teleport_Container"))))
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
			desc.vRotation = { 0.f, XMConvertToRadians(90.f), XMConvertToRadians(-45.f)};
			//desc.vRotation = { 0.f, 0.f, 0.f };

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

HRESULT CLevel_GamePlay::Ready_Layer_DoorLock()
{
	CDoorLock::DOORLOCK_DESC desc = {};

	desc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.fSpeedPerSec = 1.f;
	desc.vPos = _float3(69.f, 5.f, 634.f);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Interaction"), TEXT("Prototype_GameObject_DoorLock"), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI()
{
	CUI_PlayerBase::PLAYERBASE_DESC desc = {};
	desc.fRotationPerSec = 0.f;
	desc.fSpeedPerSec = 0.f;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_PlayerBase"), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster_Yellow()
{
	CMonster::MONSTER_DESC desc = {};
	desc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.fSpeedPerSec = 1.f;

	desc.vRotation = {};
	desc.vScale = { 1.f, 1.f, 1.f };

	desc.iStartCellIndex = 5148;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Mage"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 5180;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Mage"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 5303;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Drum"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 5471;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Construct"), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster_Pupple()
{
	CMonster::MONSTER_DESC desc = {};
	desc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.fSpeedPerSec = 1.f;

	desc.vRotation = {};
	desc.vScale = { 1.f, 1.f, 1.f };

	desc.iStartCellIndex = 6150;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Mage"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 6114;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Mage"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 4807;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Drum"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 4883;
	desc.vRotation = _float3(0.f, XMConvertToRadians(-90.f), 0.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Construct"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 4902;
	desc.vRotation = _float3(0.f, XMConvertToRadians(-90.f), 0.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Construct"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 4840;
	desc.vRotation = _float3(0.f, XMConvertToRadians(-90.f), 0.f);
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Construct"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 5941;
	desc.vRotation = {};
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Troll"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 5875;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Troll"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 5973;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Troll"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 5954;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Troll"), &desc)))
		return E_FAIL;

	desc.iStartCellIndex = 5962;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Troll"), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster_Red()
{
	CMonster::MONSTER_DESC desc = {};
	desc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.fSpeedPerSec = 1.f;

	desc.vRotation = {};
	desc.vScale = { 1.f, 1.f, 1.f };

	desc.iStartCellIndex = 4347;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Boss_Lab"), &desc)))
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
