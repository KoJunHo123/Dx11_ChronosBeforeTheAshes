#include "stdafx.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

#include "..\Public\Level_GamePlay.h"
#include "FreeCamera.h"
#include "GameInstance.h"

#include "Monster.h"
#include "Terrain.h"

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
	m_vRotationAxis = { 1.f, 0.f, 0.f, 0.f };
	m_fRotationAngle = { 90.f };

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
	const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
	static int item_current = 1;
	ImGui::ListBox("listbox", &item_current, items, IM_ARRAYSIZE(items), 8);

	// 크기
	ImGui::InputFloat3("Scale", (_float*)&m_vScale);
	// 회전축
	ImGui::InputFloat3("Rotation_Axis", (_float*)&m_vRotationAxis);
	// 회전 각도
	ImGui::InputFloat("Rotation_Angle", &m_fRotationAngle, 0.01f, 1.0f, "%.3f");

	if (m_pGameInstance->Key_Down(VK_LBUTTON))
	{
		PICKING_CHECK PickCheck = IsPicking();
		if (true == PickCheck.isPick)
		{
			Add_Monster(PickCheck.vPickPos);
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
