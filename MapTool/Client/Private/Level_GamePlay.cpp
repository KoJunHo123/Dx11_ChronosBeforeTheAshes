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
#include "Teleport.h"

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

	// 크기
	ImGui::InputFloat3("Scale", (_float*)&m_vScale);
	// 회전축
	ImGui::InputFloat3("Rotation", (_float*)&m_vRotation);
	// y 오프셋
	ImGui::InputFloat("Offset", (_float*)&m_fOffset);

	ImGui::RadioButton("Cell / 1", &m_iObjectLayer, LAYER_CELL);
	ImGui::SameLine();
	ImGui::RadioButton("Monster / 2", &m_iObjectLayer, LAYER_MONSTER);
	ImGui::SameLine();
	ImGui::RadioButton("Interaction / 3", &m_iObjectLayer, LAYER_INTERACTION);

	if (m_pGameInstance->Key_Down(0x31))
		m_iObjectLayer = LAYER_CELL;

	if (m_pGameInstance->Key_Down(0x32))
		m_iObjectLayer = LAYER_MONSTER;

	if (m_pGameInstance->Key_Down(0x33))
		m_iObjectLayer = LAYER_INTERACTION;

	ImGui::Checkbox("Delete Picking Cell", &m_bDelete_PickingCell);
	ImGui::Checkbox("Link Teleport", &m_bLink_Teleport);

	ImGui::Text("Point Count : % d", (_int)m_Points.size());

	if (m_pGameInstance->Key_Down(VK_LBUTTON) && m_pGameInstance->Key_Pressing(VK_LSHIFT))
	{
		if (LAYER_CELL == m_iObjectLayer)
		{
			if(false == m_bDelete_PickingCell)
			{
				PICKING_CHECK PickCheck = {};

				PickCheck = IsPicking_Labarynth();

				if (true == PickCheck.isPick)
				{
					_float3 vPos{};
					XMStoreFloat3(&vPos, m_pNavigation->Check_Point(PickCheck.vPickPos));	// 여기서 좌표 반환.
					m_Points.emplace_back(vPos);
				}
			}
			else
			{
				_vector vOut = {};
				m_pNavigation->Delete_PickingCell(&vOut);
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
			if (false == m_bLink_Teleport)
			{
				/*_vector vOut = {};
				_int iPickingCellIndex = m_pNavigation->Find_Index_PickingCell(&vOut);
				if (-1 != iPickingCellIndex)
				{
					Add_Interaction(vOut);
				}*/
				PICKING_CHECK PickCheck = {};
				PickCheck = IsPicking_Labarynth();
				if (true == PickCheck.isPick)
				{
					Add_Interaction(PickCheck.vPickPos);
				}
			}
			else
			{
				PICKING_CHECK PickCheck = {};

				PickCheck = IsPicking_Interaction();
			}
		}
	}

	

	if (m_Points.size() >= 3)
	{
		if(LAYER_CELL == m_iObjectLayer)
			Add_Cell();
		m_Points.clear();
	}

	if (ImGui::Button("Clear / F9") || m_pGameInstance->Key_Down(VK_F9))
	{
		switch (m_iObjectLayer)
		{
		case LAYER_MONSTER:
			m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
			break;

		case LAYER_INTERACTION:
			m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Interaction"));
			break;

		case LAYER_CELL:
			m_pNavigation->Clear_Cell();
			m_Points.clear();
			break;
		}
	}

	if (ImGui::Button("Delete / F3") || m_pGameInstance->Key_Down(VK_F3))
	{
		switch (m_iObjectLayer)
		{
		case LAYER_MONSTER:
			m_pGameInstance->Release_Object(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
			break;

		case LAYER_INTERACTION:
			m_pGameInstance->Release_Object(LEVEL_GAMEPLAY, TEXT("Layer_Interaction"));
			break;

		case LAYER_CELL:
			if(true == m_Points.empty())
				m_pNavigation->Delete_Cell();
			else
				m_Points.clear();
			break;
		}
	}

	if (ImGui::Button("Save / K") || m_pGameInstance->Key_Down('K'))
	{
		switch (m_iObjectLayer)
		{
		case LAYER_MONSTER:
			if (FAILED(Save_Monsters()))
				MSG_BOX(TEXT("Monster Save Failed"));
			else
				MSG_BOX(TEXT("Monster Save Successed"));
			break;

		case LAYER_CELL:
			if(FAILED(Save_Cells()))
				MSG_BOX(TEXT("Cell Save Failed"));
			else
				MSG_BOX(TEXT("Cell Save Successed"));
			break;

		case LAYER_INTERACTION:
			if (FAILED(Save_Interaction()))
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
		case LAYER_MONSTER:
			if (FAILED(Load_Monsters()))
				MSG_BOX(TEXT("Monster Load Failed"));
			else
				MSG_BOX(TEXT("Monster Load Successed"));
			break;

		case LAYER_CELL:
			if (FAILED(Load_Cells()))
				MSG_BOX(TEXT("Cell Load Failed"));
			else
				MSG_BOX(TEXT("Cell Load Successed"));
			break;
		case LAYER_INTERACTION:
			if (FAILED(Load_Interaction()))
				MSG_BOX(TEXT("Object Load Failed"));
			else
				MSG_BOX(TEXT("Object Load Successed"));
			break;
		}
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

CLevel_GamePlay::PICKING_CHECK CLevel_GamePlay::IsPicking_Interaction()
{
	PICKING_CHECK PickCheck = {};
	PickCheck.isPick = false;

	for(size_t i = 0; i < 6; ++i)
	{
		CTransform* pMonsterTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Interaction"), g_strTransformTag, i));
		if (nullptr == pMonsterTransform)
		{
			return PickCheck;
		}

		CModel* pModel = static_cast<CModel*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Interaction"), TEXT("Com_Model"), i));

		_uint iMeshIndex = { 0 };
		if (true == pModel->isPicking(pMonsterTransform->Get_WorldMatrix(), &PickCheck.vPickPos, &iMeshIndex))
		{
			PickCheck.isPick = true;
			Teleports.emplace_back(static_cast<CTeleport*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Interaction"), i)));

			if (2 == Teleports.size())
			{
				_float3 vPos = {};
				XMStoreFloat3(&vPos, Teleports[1]->Get_Pos());
				Teleports[0]->Set_vTeleportPos(vPos);

				XMStoreFloat3(&vPos, Teleports[0]->Get_Pos());
				Teleports[1]->Set_vTeleportPos(vPos);

				Teleports.clear();

				cout << "Success" << endl;
			}

			return PickCheck;
		}
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


HRESULT CLevel_GamePlay::Add_Cell()
{
	m_pNavigation->Add_Cell(XMLoadFloat3(&m_Points[0]), XMLoadFloat3(&m_Points[1]), XMLoadFloat3(&m_Points[2]));
	return S_OK;
}


HRESULT CLevel_GamePlay::Add_Interaction(_vector vPos)
{
	
	CPuzzleBase::PUZZLEBASE_DESC desc = {};
	
	XMStoreFloat3(&desc.vPos, vPos);
	desc.vPos.y += m_fOffset;
	desc.vScale = m_vScale;
	desc.vRotation = m_vRotation;
	desc.fRotationPerSec = 90.f;
	desc.fSpeedPerSec = 1.f;
	desc.vExtents = {};
	

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Interaction"), TEXT("Prototype_GameObject_PuzzleBase"), &desc)))
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

HRESULT CLevel_GamePlay::Save_Interaction()
{
	_char MaterialFilePath[MAX_PATH]{ "../../../Chronos/Client/Bin/SaveData/Pedestal.dat" };
	ofstream outfile(MaterialFilePath, ios::binary);

	if (!outfile.is_open())
		return E_FAIL;
	m_pGameInstance->Save_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Interaction"), &outfile);
	outfile.close();

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Interaction()
{
	_char MaterialFilePath[MAX_PATH]{ "../../../Chronos/Client/Bin/SaveData/Teleport.dat" };
	ifstream infile(MaterialFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;
	while (true)
	{
		CTeleport::TELEPORT_DESC desc{};

		infile.read(reinterpret_cast<_char*>(&desc), sizeof(CTeleport::TELEPORT_DESC));

		if (true == infile.eof())
			break;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Interaction"), TEXT("Prototype_GameObject_Teleport"), &desc)))
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
