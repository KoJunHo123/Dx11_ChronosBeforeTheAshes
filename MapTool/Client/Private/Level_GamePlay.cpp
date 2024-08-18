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

	m_vScale = { 1.f, 1.f, 1.f, 0.f };
	m_vRotationAxis = { 0.f, 1.f, 0.f, 0.f };
	m_fRotationAngle = { 0.f };

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

	// 어떤 모델 부를지 선택
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

	// 크기
	ImGui::InputFloat3("Scale", (_float*)&m_vScale);
	// 회전축
	ImGui::InputFloat3("Rotation_Axis", (_float*)&m_vRotationAxis);
	// 회전 각도
	ImGui::InputFloat("Rotation_Angle", &m_fRotationAngle, 0.01f, 1.0f, "%.3f");

	
	ImGui::RadioButton("Player", &m_iObjectLayer, LAYER_PLAYER); 
	ImGui::SameLine();
	ImGui::RadioButton("Monster", &m_iObjectLayer, LAYER_MONSTER); 
	//ImGui::SameLine();
	//ImGui::RadioButton("radio c", &iObjectLayer, 2);

	

	if (m_pGameInstance->Key_Down(VK_LBUTTON))
	{
		PICKING_CHECK PickCheck = IsPicking();
		if (true == PickCheck.isPick)
		{
			switch (m_iObjectLayer)
			{
			case LAYER_PLAYER:
				if(nullptr == m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), g_strTransformTag, 0))
					Add_Player(PickCheck.vPickPos);
				break;

			case LAYER_MONSTER:
				Add_Monster(PickCheck.vPickPos);
				break;
			}
				
		}
	}

	if (ImGui::Button("Clear"))
	{
		switch (m_iObjectLayer)
		{
		case LAYER_PLAYER:
			m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
			break;

		case LAYER_MONSTER:
			m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
			break;
		}
		
	}

	if (ImGui::Button("Save"))
	{
		switch (m_iObjectLayer)
		{
		case LAYER_PLAYER:
			if (FAILED(Save_Player()))
				MSG_BOX(TEXT("Save Failed"));
			break;

		case LAYER_MONSTER:
			if (FAILED(Save_Monsters()))
				MSG_BOX(TEXT("Save Failed"));
			break;
		}
	}

	if (ImGui::Button("Load"))
	{
		switch (m_iObjectLayer)
		{
		case LAYER_PLAYER:
			if (FAILED(Load_Player()))
				MSG_BOX(TEXT("Save Failed"));
			break;

		case LAYER_MONSTER:
			if (FAILED(Load_Monsters()))
				MSG_BOX(TEXT("Save Failed"));
			break;
		}
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

	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Labyrinth"))))
	//	return E_FAIL;

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

HRESULT CLevel_GamePlay::Add_Monster(_vector vPos)
{
	if (0.f == m_vRotationAxis.x && 0.f == m_vRotationAxis.y && 0.f == m_vRotationAxis.z)
		return E_FAIL;

	CMonster::MONSTER_DESC desc = {};

	desc.vPos = vPos;
	desc.vScale = m_vScale;
	desc.vRotationAxis = XMLoadFloat4(&m_vRotationAxis);
	desc.fRotationAngle = m_fRotationAngle;
	desc.fRotationPerSec = 0.f;
	desc.fSpeedPerSec = 0.f;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster"), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Add_Player(_vector vPos)
{
	if (0.f == m_vRotationAxis.x && 0.f == m_vRotationAxis.y && 0.f == m_vRotationAxis.z)
		return E_FAIL;

	CPlayer::PLAYER_DESC desc = {};

	desc.vPos = vPos;
	desc.vScale = m_vScale;
	desc.vRotationAxis = XMLoadFloat4(&m_vRotationAxis);
	desc.fRotationAngle = m_fRotationAngle;
	desc.fRotationPerSec = 0.f;
	desc.fSpeedPerSec = 0.f;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &desc)))
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
	m_pGameInstance->Create_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));

	_char MaterialFilePath[MAX_PATH]{ "../../../Chronos/Client/Bin/SaveData/Monster.dat" };
	ifstream infile(MaterialFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;
	while(true)
	{
		Add_Monster(XMVectorSet(0.f, 0.f, 0.f, 0.f));
		if(FAILED(m_pGameInstance->Load_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), &infile)))
		{
			m_pGameInstance->Release_Object(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
			break;
		}
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
	m_pGameInstance->Create_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	_char MaterialFilePath[MAX_PATH]{ "../../../Chronos/Client/Bin/SaveData/Player.dat" };
	ifstream infile(MaterialFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;
	while (true)
	{
		Add_Player(XMVectorSet(0.f, 0.f, 0.f, 0.f));
		if(FAILED(m_pGameInstance->Load_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), &infile)))
		{
			m_pGameInstance->Release_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
			break;
		}
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


}
