#include "stdafx.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

#include "..\Public\Level_GamePlay.h"
#include "FreeCamera.h"
#include "GameInstance.h"

#include "Monster.h"
#include "Player.h"
#include "Terrain.h"
#include "Labyrinth.h"
#include "PuzzleBase.h"

#include "Layer.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	m_vScale = { 1.f, 1.f, 1.f };
	m_vRotation = { 0.f, 0.f, 0.f };

	m_pNavigation = static_cast<CNavigation*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Navigation"), 0));
	Safe_AddRef(m_pNavigation);

	/*CVIBuffer_Terrain* pTerrainBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"), 0));
	CTransform* pTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), g_strTransformTag, 0));

	pTerrainBuffer->Create_Cells(m_pNavigation, pTransform->Get_State(CTransform::STATE_POSITION));*/

	m_PrototypeKeys = m_pGameInstance->Get_PrototypeKeys();

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	
}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	bool bDemo = true;
	ImGui::ShowDemoWindow(&bDemo);

	ImGui::Begin("Map Editor");


	//// 어떤 모델 부를지 선택
	//const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
	//static int item_current = 1;
	//ImGui::ListBox("listbox", &item_current, items, IM_ARRAYSIZE(items), 8);

	CTransform* pTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), g_strTransformTag, 0));
	
	if(nullptr != pTransform)
	{
		XMStoreFloat4(&m_vCheckPos, pTransform->Get_State(CTransform::STATE_POSITION));
		ImGui::Text("Current Player Pos X: %f", m_vCheckPos.x);
		ImGui::Text("Current Player Pos Y: %f", m_vCheckPos.y);
		ImGui::Text("Current Player Pos Z: %f", m_vCheckPos.z);
	}

	_uint iNum = static_cast<CLabyrinth*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"))->Find_GameObject(1))->Get_ColliderCount();
	ImGui::Text("Labyrinth Collider Count: %d", iNum);

	// 크기
	ImGui::InputFloat3("Scale", (_float*)&m_vScale);
	// 회전축
	ImGui::InputFloat3("Rotation", (_float*)&m_vRotation);
	// 콜라이더 크기
	ImGui::InputFloat3("Extent", (_float*)&m_vExtents);
	// y 오프셋
	ImGui::InputFloat("Offset", (_float*)&m_fOffset);

	ImGui::RadioButton("Cell / 1", &m_iObjectLayer, CELL);
	ImGui::SameLine();
	ImGui::RadioButton("Collider / 2", &m_iObjectLayer, COLLIDER);

	ImGui::RadioButton("Player / 3", &m_iObjectLayer, LAYER_PLAYER); 
	ImGui::SameLine();
	ImGui::RadioButton("Monster / 4", &m_iObjectLayer, LAYER_MONSTER);
	
	ImGui::RadioButton("WallCell / 5", &m_iObjectLayer, WALLCELL);
	ImGui::SameLine();
	ImGui::RadioButton("Object / 6", &m_iObjectLayer, LAYER_INTERACTION);

	if (m_pGameInstance->Key_Down(0x31))
		m_iObjectLayer = CELL;

	if (m_pGameInstance->Key_Down(0x32))
		m_iObjectLayer = COLLIDER;

	if (m_pGameInstance->Key_Down(0x33))
		m_iObjectLayer = LAYER_PLAYER;

	if (m_pGameInstance->Key_Down(0x34))
		m_iObjectLayer = LAYER_MONSTER;

	if (m_pGameInstance->Key_Down(0x35))
		m_iObjectLayer = WALLCELL;

	if (m_pGameInstance->Key_Down(0x36))
		m_iObjectLayer = LAYER_INTERACTION;

	ImGui::RadioButton("Terrain / Z", &m_iPickingObject, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Labarynth / X", &m_iPickingObject, 1);

	if (m_pGameInstance->Key_Down('Z'))
		m_iPickingObject = 0;

	if (m_pGameInstance->Key_Down('X'))
		m_iPickingObject = 1;

	ImGui::RadioButton("Add / F1", &m_iPickingEffect, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Delete / F2", &m_iPickingEffect, 1);
	ImGui::RadioButton("Change_Type / F10", &m_iPickingEffect, 2);

	if (m_pGameInstance->Key_Down(VK_F1))
		m_iPickingEffect = 0;

	if (m_pGameInstance->Key_Down(VK_F2))
		m_iPickingEffect = 1;

	if (m_pGameInstance->Key_Down(VK_F10))
		m_iPickingEffect = 2;

	ImGui::Text("Point Count : % d", (_int)m_Points.size());

	if (m_pGameInstance->Key_Down(VK_LBUTTON) && m_pGameInstance->Key_Pressing(VK_LSHIFT))
	{
		if(0 == m_iPickingEffect)
		{
			if (CELL == m_iObjectLayer)
			{
				PICKING_CHECK PickCheck = {};

				if (0 == m_iPickingObject)
				{
					PickCheck = IsPicking();
				}
				else if (1 == m_iPickingObject)
				{
					PickCheck = IsPicking_Labarynth();
				}

				if (true == PickCheck.isPick)
				{
					_float3 vPos{};
					XMStoreFloat3(&vPos, m_pNavigation->Check_Point(PickCheck.vPickPos));	// 여기서 좌표 반환.
					m_Points.emplace_back(vPos);
				}
			}
			else if (COLLIDER == m_iObjectLayer)
			{
				PICKING_CHECK PickCheck = {};

				if (1 == m_iPickingObject)
					PickCheck = IsPicking_Labarynth();

				if (true == PickCheck.isPick)
				{
					if(FAILED(Add_Collider(PickCheck.vPickPos)))
						MSG_BOX(TEXT("콜라이더 안된다요..."));
				}
			}
			else if (WALLCELL == m_iObjectLayer)
			{
				PICKING_CHECK PickCheck = {};
				if (1 == m_iPickingObject)
				{
					PickCheck = IsPicking_Labarynth();
				}
				if (true == PickCheck.isPick)
				{
					_float3 vPos{};
					XMStoreFloat3(&vPos, m_pNavigation->Check_Point(PickCheck.vPickPos));	// 여기서 좌표 반환.
					m_Points.emplace_back(vPos);
				}

			}
			else if (LAYER_PLAYER == m_iObjectLayer)
			{
				_vector vOut = {};
				_int iPickingCellIndex = m_pNavigation->Find_Index_PickingCell(&vOut);
				if(-1 != iPickingCellIndex)
				{
					if (nullptr == m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), g_strTransformTag, 0))
						Add_Player(vOut, iPickingCellIndex);
				}
			}
			else if (LAYER_MONSTER == m_iObjectLayer)
			{
				_vector vOut = {};
				_int iPickingCellIndex = m_pNavigation->Find_Index_PickingCell(&vOut);
				if (-1 != iPickingCellIndex)
				{
					Add_Monster(vOut, iPickingCellIndex);
				}
			}
			else if (LAYER_INTERACTION == m_iObjectLayer)
			{
				_vector vOut = {};
				_int iPickingCellIndex = m_pNavigation->Find_Index_PickingCell(&vOut);
				if (-1 != iPickingCellIndex)
				{
					Add_Object(vOut);
				}

			}
		}
		else if (1 == m_iPickingEffect)
		{
			if (CELL == m_iObjectLayer )
			{
				_vector vPos = {};
				m_pNavigation->Delete_PickingCell(&vPos);
			}
			else if (WALLCELL == m_iObjectLayer)
			{
				m_pNavigation->Delete_PickingWallCell();
			}
		}
		else if (2 == m_iPickingEffect)
		{
			if (CELL == m_iObjectLayer)
			{
				_vector vPos = {};
				m_pNavigation->Change_CellType(&vPos);
			}
		}
	}

	if (m_Points.size() >= 3)
	{
		if(CELL == m_iObjectLayer)
			Add_Cell();
		else if (WALLCELL == m_iObjectLayer)
			m_pNavigation->Add_WallCell(XMLoadFloat3(&m_Points[0]), XMLoadFloat3(&m_Points[1]), XMLoadFloat3(&m_Points[2]));
		m_Points.clear();
	}

	if (ImGui::Button("Clear / F9") || m_pGameInstance->Key_Down(VK_F9))
	{
		switch (m_iObjectLayer)
		{
		case LAYER_PLAYER:
			m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
			break;

		case LAYER_MONSTER:
			m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
			break;

		case CELL:
			m_pNavigation->Clear_Cell();
			m_Points.clear();
			break;
		}
	}

	if (ImGui::Button("Delete / F3") || m_pGameInstance->Key_Down(VK_F3))
	{
		switch (m_iObjectLayer)
		{
		case LAYER_PLAYER:
			m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
			break;

		case LAYER_MONSTER:
			m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
			break;

		case CELL:
			if(true == m_Points.empty())
				m_pNavigation->Delete_Cell();
			else
				m_Points.clear();
			break;
		case COLLIDER:
			static_cast<CLabyrinth*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"))->Find_GameObject(1))->Release_LastCollider();
			break;
		case LAYER_INTERACTION:
			m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Puzzle"));
			break;
		}
	}

	if (ImGui::Button("Save / K") || m_pGameInstance->Key_Down('K'))
	{
		switch (m_iObjectLayer)
		{
		case LAYER_PLAYER:
			if (FAILED(Save_Player()))
				MSG_BOX(TEXT("Player Save Failed"));
			else
				MSG_BOX(TEXT("Player Save Successed"));
			break;

		case LAYER_MONSTER:
			if (FAILED(Save_Monsters()))
				MSG_BOX(TEXT("Monster Save Failed"));
			else
				MSG_BOX(TEXT("Monster Save Successed"));
			break;

		case CELL:
			if(FAILED(Save_Cells()))
				MSG_BOX(TEXT("Cell Save Failed"));
			else
				MSG_BOX(TEXT("Cell Save Successed"));
			break;

		case LAYER_INTERACTION:
			if (FAILED(Save_Objects()))
				MSG_BOX(TEXT("Object Save Failed"));
			else
				MSG_BOX(TEXT("Object Save Successed"));
			break;
		}
	}

	if (ImGui::Button("Load / L") || m_pGameInstance->Key_Down('L'))
	{
		switch (m_iObjectLayer)
		{
		case LAYER_PLAYER:
			if (FAILED(Load_Player()))
				MSG_BOX(TEXT("Player Load Failed"));
			else
				MSG_BOX(TEXT("Player Load Successed"));
			break;

		case LAYER_MONSTER:
			if (FAILED(Load_Monsters()))
				MSG_BOX(TEXT("Monster Load Failed"));
			else
				MSG_BOX(TEXT("Monster Load Successed"));
			break;

		case CELL:
			if (FAILED(Load_Cells()))
				MSG_BOX(TEXT("Cell Load Failed"));
			else
				MSG_BOX(TEXT("Cell Load Successed"));
			break;
		case LAYER_INTERACTION:
			if (FAILED(Load_Objects()))
				MSG_BOX(TEXT("Object Save Failed"));
			else
				MSG_BOX(TEXT("Object Save Successed"));
			break;
		}
	}

	if (ImGui::Button("Create Cell On Terrain / F6") || m_pGameInstance->Key_Down(VK_F6))
	{
		CVIBuffer_Terrain* pTerrainBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"), 0));
		CTransform* pTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), g_strTransformTag, 0));

		pTerrainBuffer->Create_Cells(m_pNavigation, pTransform->Get_State(CTransform::STATE_POSITION));
	}

	if (ImGui::Button("Create Cell On Navigation / F7") || m_pGameInstance->Key_Down(VK_F7))
	{
		CModel* pLabyrinthModel = static_cast<CModel*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Model"), 1));
		CTransform* pTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), g_strTransformTag, 1));

		pLabyrinthModel->Create_Cells(m_pNavigation, pTransform->Get_State(CTransform::STATE_POSITION));
	}

	if (ImGui::Button("Create WallCell On Navigation / F8") || m_pGameInstance->Key_Down(VK_F8))
	{
		m_pNavigation->Create_WallCell_FromCell();
	}


	if (ImGui::Button("Render On Off / R") || m_pGameInstance->Key_Down('R'))
	{
		CModel* pLabyrinthModel = static_cast<CModel*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Model"), 1));
		pLabyrinthModel->Render_OnOff();
	}

	ImGui::End();

	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

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

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Labyrinth"))))
		return E_FAIL;

	return S_OK;
}

CLevel_GamePlay::PICKING_CHECK CLevel_GamePlay::IsPicking()
{
	PICKING_CHECK PickCheck = {};
	PickCheck.isPick = false;

	CTransform* pTerrainTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), g_strTransformTag, 0));
	CVIBuffer_Terrain* pTerrainVIBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"),
		TEXT("Com_VIBuffer"), 0));
	if (true == pTerrainVIBuffer->isPicking(pTerrainTransform->Get_WorldMatrix(), &PickCheck.vPickPos))
	{
		PickCheck.isPick = true;
	}

	return PickCheck;
}

CLevel_GamePlay::PICKING_CHECK CLevel_GamePlay::IsPicking_Labarynth()
{
	PICKING_CHECK PickCheck = {};
	PickCheck.isPick = false;

	CTransform* pMonsterTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), g_strTransformTag, 1));
	if (nullptr == pMonsterTransform)
	{
		return PickCheck;
	}

	CModel* pModel = static_cast<CModel*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Model"), 1));

	_uint iMeshIndex = { 0 };
	if (true == pModel->isPicking(pMonsterTransform->Get_WorldMatrix(), &PickCheck.vPickPos, &iMeshIndex))
	{
		PickCheck.isPick = true;
	}

	return PickCheck;
}

HRESULT CLevel_GamePlay::Add_Monster(_vector vPos, _int iCellIndex)
{
	CMonster::MONSTER_DESC desc = {};

	XMStoreFloat3(&desc.vPos, vPos);
	desc.vScale = m_vScale;
	desc.vRotation = m_vRotation;
	desc.fRotationPerSec = 90.f;
	desc.fSpeedPerSec = 1.f;
	desc.iStartCellIndex = iCellIndex;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster"), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Add_Player(_vector vPos, _int iCellIndex)
{
	CPlayer::PLAYER_DESC desc = {};

	XMStoreFloat3(&desc.vPos, vPos);
	desc.vScale = m_vScale;
	desc.vRotation = m_vRotation;
	desc.fRotationPerSec = 90.f;
	desc.fSpeedPerSec = 1.f;
	desc.iStartCellIndex = iCellIndex;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Add_Cell()
{
	m_pNavigation->Add_Cell(XMLoadFloat3(&m_Points[0]), XMLoadFloat3(&m_Points[1]), XMLoadFloat3(&m_Points[2]));
	return S_OK;
}

HRESULT CLevel_GamePlay::Add_Collider(_fvector vPos)
{
	if (m_vExtents.x == 0.f || m_vExtents.y == 0.f || m_vExtents.z == 0.f)
		return E_FAIL;

	if(FAILED(static_cast<CLabyrinth*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"))->Find_GameObject(1))->Add_Collider(vPos, XMLoadFloat3(&m_vExtents))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Add_Object(_vector vPos)
{
	CPuzzleBase::PUZZLEBASE_DESC desc = {};

	XMStoreFloat3(&desc.vPos, vPos);
	desc.vPos.y += m_fOffset;
	desc.vScale = m_vScale;
	desc.vRotation = m_vRotation;
	desc.fRotationPerSec = 90.f;
	desc.fSpeedPerSec = 1.f;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Puzzle"), TEXT("Prototype_GameObject_PuzzleBase"), &desc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_GamePlay::Save_Monsters()
{
	_char MaterialFilePath[MAX_PATH]{"../../../Chronos/Client/Bin/SaveData/Monster.dat"};
	ofstream outfile(MaterialFilePath, ios::binary);

	if (!outfile.is_open())
		return E_FAIL;
	m_pGameInstance->Save_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &outfile);
	outfile.close();

	return S_OK;
}

// 맵툴
HRESULT CLevel_GamePlay::Load_Monsters()
{
	_char MaterialFilePath[MAX_PATH]{ "../../../Chronos/Client/Bin/SaveData/Monster.dat" };
	ifstream infile(MaterialFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;
	while(true)
	{
		CMonster::MONSTER_DESC desc{};

		infile.read(reinterpret_cast<_char*>(&desc), sizeof(CMonster::MONSTER_DESC));

		if (true == infile.eof())
			break;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster"), &desc)))
			return E_FAIL;
	}

	infile.close();

	return S_OK;
}

HRESULT CLevel_GamePlay::Save_Player()
{
	_char MaterialFilePath[MAX_PATH]{ "../../../Chronos/Client/Bin/SaveData/Player.dat" };
	ofstream outfile(MaterialFilePath, ios::binary);
	 
	if (!outfile.is_open())
		return E_FAIL;
	m_pGameInstance->Save_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), &outfile);
	outfile.close();

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Player()
{
	_char MaterialFilePath[MAX_PATH]{ "../../../Chronos/Client/Bin/SaveData/Player.dat" };
	ifstream infile(MaterialFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;
	while (true)
	{
		CPlayer::PLAYER_DESC desc{};

		infile.read(reinterpret_cast<_char*>(&desc), sizeof(CPlayer::PLAYER_DESC));

		if (true == infile.eof())
			break;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &desc)))
			return E_FAIL;

	}

	infile.close();

	return S_OK;
}

HRESULT CLevel_GamePlay::Save_Cells()
{
	if(FAILED(m_pNavigation->Save_Data()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Cells()
{
	if (FAILED(m_pNavigation->Load_Data()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Save_Objects()
{
	_char MaterialFilePath[MAX_PATH]{ "../../../Chronos/Client/Bin/SaveData/PuzzleBase.dat" };
	ofstream outfile(MaterialFilePath, ios::binary);

	if (!outfile.is_open())
		return E_FAIL;
	m_pGameInstance->Save_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Puzzle"), &outfile);
	outfile.close();

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Objects()
{
	_char MaterialFilePath[MAX_PATH]{ "../../../Chronos/Client/Bin/SaveData/PuzzleBase.dat" };
	ifstream infile(MaterialFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;
	while (true)
	{
		CPuzzleBase::PUZZLEBASE_DESC desc{};

		infile.read(reinterpret_cast<_char*>(&desc), sizeof(CPuzzleBase::PUZZLEBASE_DESC));

		if (true == infile.eof())
			break;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Puzzle"), TEXT("Prototype_GameObject_PuzzleBase"), &desc)))
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

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pNavigation);
}
