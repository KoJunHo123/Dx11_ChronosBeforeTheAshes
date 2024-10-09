#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"

// 오브젝트

// UI : 로고
#include "BackGround.h"
#include "UI_GlowStar.h"
#include "UI_LogoTitle.h"
#include "UI_LogoMaze.h"
#include "UI_TextBox.h"

// UI : 게임플레이
#include "UI_PlayerBase.h"
#include "UI_DragonHeart.h"
#include "UI_DragonStone.h"
#include "UI_GageBar.h"
#include "UI_MonsterHPBar.h"
#include "UI_BossHPBar.h"
#include "UI_BossHPBarDeco.h"
#include "UI_BossHPBarGlow.h"

// 지형
#include "Labyrinth.h"
#include "Terrain.h"
#include "FloorChunk.h"
#include "Sky.h"

// 퍼즐
#include "PuzzleBase.h"
#include "PuzzlePart.h"
#include "Puzzle_InterColl.h"

// 카메라
#include "Camera_Container.h"
#include "Camera_Shorder.h"
#include "Camera_Interaction.h"

// 플레이어
#include "Player.h"
#include "Player_Body.h"
#include "Player_Weapon.h"
#include "Player_Shield.h"
#include "Player_Item.h"

// 몬스터
#include "Particle_Monster_Death.h"

// 보스몬스터
#include "Boss_Lab.h"
#include "Boss_Lab_Body.h"
#include "Boss_Lab_Attack.h"
#include "Boss_Lab_Teleport_Smoke.h"
#include "Boss_Lab_Teleport_Stone.h"

// 몬스터 : 석상
#include "Lab_Construct.h"
#include "Lab_Construct_Body.h"
#include "Lab_Construct_Attack.h"

// 몬스터 : 드럼
#include "Lab_Drum.h"
#include "Lab_Drum_Body.h"
#include "Lab_Drum_Attack.h"

// 몬스터 : 메이지
#include "Lab_Mage.h"
#include "Lab_Mage_Body.h"
#include "Lab_Mage_Attack.h"

// 몬스터 : 트롤
#include "Lab_Troll.h"
#include "Lab_Troll_Body.h"
#include "Lab_Troll_Weapon.h"

// 상호작용 : 텔레포트
#include "Teleport.h"
#include "Teleport_Container.h"

// 상호작용 : 거치대
#include "Pedestal.h"
#include "Pedestal_Item.h"
#include "Pedestal_InterColl.h"

// 상호작용 : 웨이포인트
#include "WayPoint.h"
#include "WayPoint_InterColl.h"

// 상호작용 : 도어락
#include "DoorLock.h"
#include "DoorLock_InterColl.h"
#include "DoorLock_Effect.h"

// 파티클
#include "Particle_AttackLight.h"
#include "Particle_Spawn.h"
#include "Particle_Snow.h"
#include "Particle_Smoke.h"
#include "Particle_LaunchStone.h"
#include "Particle_LaunchWaterDrop.h"

// 이펙트
#include "Effect_Flash.h"
#include "Effect_Spark.h"
#include "Effect_BloodCore.h"
#include "Effect_BloodSpray.h"
#include "Effect_Flare.h"

// 트레일
#include "Trail_Revolve.h"

// 인벤토리
#include "Inventory.h"

// 아이템
#include "DragonHeart.h"
#include "Puzzle_Item.h"
#include "RuneKey.h"


CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = static_cast<CLoader*>(pArg);

	if(FAILED(pLoader->Loading()))
		return 1;
		
	return 0;
}


HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Ready_Resources_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Ready_Resources_For_GamePlayLevel();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	return hr;
}

void CLoader::Draw_LoadingText()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, m_szLoadingText);
#endif
}

HRESULT CLoader::Ready_Resources_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
#pragma region BACKGROUND
	/* For. Prototype_Component_Texture_UI_MainMenu_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_MainMenu_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MainMenu_BackGround.png"), 1))))
		return E_FAIL;
#pragma endregion
#pragma region UI
	/* For. Prototype_Component_Texture_UI_Glow_Star */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_Glow_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Glow_Star.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Logo_Title */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_Logo_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Main_TextBox */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_TextBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Main_TextBox.png"), 1))))
		return E_FAIL;
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
#pragma region UI
	/* For. Prototype_GameObject_UI_GlowStar */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_GlowStar"),
		CUI_GlowStar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_UI_LogoTitle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LogoTitle"),
		CUI_LogoTitle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_UI_LogoMaze */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LogoMaze"),
		CUI_LogoMaze::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_UI_TextBox */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_TextBox"),
		CUI_TextBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
#pragma region TEXTURE
	/* For. Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_4.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Noise */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Noise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Noise/Noise_%d.png"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Particle_Stone */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Stone"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/VFX/Particle/Stone.png"), 1))))
		return E_FAIL;
	
	/* For. Prototype_Component_Texture_Particle_LightLong */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_LightLong"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/VFX/Particle/T_LightLong_A.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Particle_Spark */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Spark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/VFX/Particle/Particle_Spark.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/VFX/Particle/Snow.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Teleport */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Teleport"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/VFX/Effect/Teleport.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Glow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Glow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/VFX/Effect/Glow_3.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Flare */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Flare"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/VFX/Effect/T_Flare_%d.png"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Smoke */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Smoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/VFX/Effect/Smoke_RGB.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Spark */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Spark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/VFX/Effect/Spark.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_BloodCore */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BloodCore"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/VFX/Effect/BloodCore_%d.png"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_BloodCore_N */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BloodCore_N"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/VFX/Effect/BloodCore_%d_N.png"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_BloodSpray */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BloodSpray"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/VFX/Effect/BloodSpray_%d.png"), 6))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_BloodSpray_N */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BloodSpray_N"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/VFX/Effect/BloodSpray_%d_N.png"), 6))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_DragonHeart_Core */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DragonHeart_Core"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/heal_item_icon.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_DragonHeart_Border */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DragonHeart_Border"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_UI_HUD_Border_DragonHeart.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_DragonStone_Core_Red */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DragonStone_Core_Red"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/fire_stone_icon.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_DragonStone_Core_Pupple */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DragonStone_Core_Pupple"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/shadow_stone_icon.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_DragonStone_Border */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DragonStone_Border"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_UI_HUD_Border_Stone.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_GageBar_Border */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_GageBar_Border"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_UI_HUD_Border_Bars.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Pack */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Pack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_UI_Pack_01.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Monster_HPBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Monster_HPBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Monster_HPBar.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Inventory */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Inventory"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_UI_Inventory.png"), 1))))
		return E_FAIL;

	// 여기서부터 보스 UI
	/* For. Prototype_Component_Texture_UI_Boss_HPBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Boss_HPBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_2DHUD_BarBackground_Boss.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Boss_BarDeco */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Boss_BarDeco"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_2DHUD_BossBarDecor.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Glow_Up */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Glow_Up"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_UI_Glow_01.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Particle_Smoke_2x2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Smoke_2x2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/VFX/Particle/T_EFF_Smoke_25_M.dds"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Trail_Beam */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Trail_Beam"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/VFX/Trail/T_Beam_Core_Default.png"), 1))))
		return E_FAIL;

#pragma endregion
	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));
#pragma region VIBUFFER
#pragma region DEFAULT
	/* For. Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Particle_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Rect"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region POINT_INSTANCE
	if(FAILED(Load_Particle()))
		return E_FAIL;

	CVIBuffer_Instancing::INSTANCE_DESC			ParticleDesc{};
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);
	/* For. Prototype_Component_VIBuffer_Particle_AttackLight */
	ParticleDesc.iNumInstance = 20;
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vRange = _float3(0.3f, 0.3f, 0.3f);	// 이게 첫 생성 범위
	ParticleDesc.vExceptRange = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSize = _float2(0.3f, 0.6f);		// 이게 크기
	//ParticleDesc.vSize = _float2(1.f, 2.f);		// 이게 크기
	ParticleDesc.vSpeed = _float2(10.f, 15.f);
	ParticleDesc.vLifeTime = _float2(0.2f, 0.4f);
	ParticleDesc.vMinColor = _float4(1.f, 0.647f, 0.f, 1.f);
	ParticleDesc.vMaxColor = _float4(1.f, 1.f, 0.4f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_AttackLight"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Particle_Snow */
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);

	ParticleDesc.iNumInstance = 3000;
	ParticleDesc.vCenter = _float3(64.f, 20.f, 64.f);
	ParticleDesc.vRange = _float3(128.f, 1.f, 128.f);
	ParticleDesc.vExceptRange = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSize = _float2(0.1f, 0.3f);
	ParticleDesc.vSpeed = _float2(1.f, 3.f);
	ParticleDesc.vLifeTime = _float2(4.f, 8.f);
	ParticleDesc.vMinColor = _float4(1.f, 1.f, 1.f, 1.f);
	ParticleDesc.vMaxColor = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Snow"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Particle_Boss_Teleport_Stone */
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);

	ParticleDesc.iNumInstance = 1000;
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vRange = _float3(5.f, 1.f, 5.f);
	ParticleDesc.vExceptRange = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSize = _float2(0.5f, 0.7f);
	ParticleDesc.vSpeed = _float2(5.f, 10.f);
	ParticleDesc.vLifeTime = _float2(1.f, 2.f);
	ParticleDesc.vMinColor = _float4(0.f, 0.f, 0.f, 0.f);
	ParticleDesc.vMaxColor = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Boss_Teleport_Stone"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Particle_Launch_Stone */
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);

	ParticleDesc.iNumInstance = 500;
	ParticleDesc.vCenter = _float3(0.f, 1.f, 0.f);
	ParticleDesc.vRange = _float3(5.f, 1.f, 5.f);
	ParticleDesc.vExceptRange = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSize = _float2(0.2f, 0.4f);
	ParticleDesc.vSpeed = _float2(10.f, 20.f);
	ParticleDesc.vLifeTime = _float2(1.f, 2.f);
	ParticleDesc.vMinColor = _float4(0.f, 0.f, 0.f, 0.f);
	ParticleDesc.vMaxColor = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Launch_Stone"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Particle_Launch_WaterDrop */
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);

	ParticleDesc.iNumInstance = 500;
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vRange = _float3(5.f, 1.f, 5.f);
	ParticleDesc.vExceptRange = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSize = _float2(0.2f, 0.4f);
	ParticleDesc.vSpeed = _float2(15.f, 20.f);
	ParticleDesc.vLifeTime = _float2(2.f, 4.f);
	ParticleDesc.vMinColor = _float4(0.416f, 0.353f, 0.804f, 0.f);
	ParticleDesc.vMaxColor = _float4(0.482f, 0.408f, 9.933f, 1.f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Launch_WaterDrop"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;
#pragma endregion
#pragma region TRAIL_INSTANCE
	CVIBuffer_Instancing::INSTANCE_DESC TrailInstance = {};
	ZeroMemory(&TrailInstance, sizeof TrailInstance);

	TrailInstance.iNumInstance = 30;
	TrailInstance.vLifeTime = _float2(0.4f, 0.8f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Trail_Test"),
		CVIBuffer_Trail_Instance::Create(m_pDevice, m_pContext, TrailInstance))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Trail_TwoPoint_Test"),
		CVIBuffer_Trail_TwoPoint_Instance::Create(m_pDevice, m_pContext, TrailInstance))))
		return E_FAIL;

#pragma endregion
#pragma endregion
#pragma region LABYRINTH
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	/* For. Prototype_Component_Model_Labyrinth*/
#ifndef _DEBUG
	PreTransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Labyrinth"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/Labyrinth/Labyrinth", PreTransformMatrix))))
		return E_FAIL;
#endif

	/* For. Prototype_Component_Model_FloorChunk_A */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FloorChunk_A"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/FloorChunk/FloorChunk_A", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_FloorChunk_B */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FloorChunk_B"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/FloorChunk/FloorChunk_B", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_FloorChunk_C */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FloorChunk_C"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/FloorChunk/FloorChunk_C", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_FloorChunk_D */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FloorChunk_D"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/FloorChunk/FloorChunk_D", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_FloorChunk_E */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FloorChunk_E"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/FloorChunk/FloorChunk_E", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Pedestal */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Pedestal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/Pedestal/Pedestal", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Teleport */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Teleport"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/Teleport/Teleport_A", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Crystal */
	PreTransformMatrix = XMMatrixScaling(5.f, 5.f, 5.f) * XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Crystal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/Crystal/Crystal", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_DoorLock */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_DoorLock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/DoorLock/DoorLock", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion
#pragma region PUZZLE
	/* For. Prototype_Component_Model_Puzzle_Base */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Puzzle_Base"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/Puzzle/Puzzle_Base", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Puzzle_Piece_00 */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Puzzle_Piece_00"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/Puzzle/Puzzle_Piece_00", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Puzzle_Piece_01 */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Puzzle_Piece_01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/Puzzle/Puzzle_Piece_01", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Puzzle_Piece_02 */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Puzzle_Piece_02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/Puzzle/Puzzle_Piece_02", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Puzzle_Piece_10 */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Puzzle_Piece_10"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/Puzzle/Puzzle_Piece_10", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Puzzle_Piece_11 */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Puzzle_Piece_11"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/Puzzle/Puzzle_Piece_11", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Puzzle_Piece_12 */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Puzzle_Piece_12"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/Puzzle/Puzzle_Piece_12", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Puzzle_Piece_21 */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Puzzle_Piece_21"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/Puzzle/Puzzle_Piece_21", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Puzzle_Piece_22 */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Puzzle_Piece_22"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/Puzzle/Puzzle_Piece_22", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Puzzle_ReplacementPiece */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Puzzle_ReplacementPiece"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/Puzzle/Puzzle_ReplacementPiece", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion
#pragma region PLAYER
	/* For. Prototype_Component_Model_Player*/
	 PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	 if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		 CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/Player",PreTransformMatrix))))
		 return E_FAIL;

	 /* For. Prototype_Component_Model_Player_Sword*/
	 PreTransformMatrix = XMMatrixIdentity();
	 if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Sword"),
		 CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Player/Player_Sword/Player_Sword", PreTransformMatrix))))
		 return E_FAIL;

	 /* For. Prototype_Component_Model_Player_Shield*/
	 PreTransformMatrix = XMMatrixIdentity();
	 if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Shield"),
		 CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Player/Player_Shield/Player_Shield", PreTransformMatrix))))
		 return E_FAIL;
#pragma endregion
#pragma region MONSTER
	 /* For. Prototype_Component_Model_Monster_Boss_Lab*/
	 PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	 if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Boss_Lab"),
		 CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Boss_Lab/Boss_Lab", PreTransformMatrix))))
		 return E_FAIL;

	 /* For. Prototype_Component_Model_Monster_Lab_Construct*/
	 PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	 if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Lab_Construct"),
		 CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Lab_Construct/Lab_Construct", PreTransformMatrix))))
		 return E_FAIL;

	 /* For. Prototype_Component_Model_Monster_Lab_Drum*/
	 PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	 if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Lab_Drum"),
		 CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Lab_Drum/Lab_Drum", PreTransformMatrix))))
		 return E_FAIL;

	 /* For. Prototype_Component_Model_Monster_Lab_Mage*/
	 PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	 if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Lab_Mage"),
		 CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Lab_Mage/Lab_Mage", PreTransformMatrix))))
		 return E_FAIL;

	 /* For. Prototype_Component_Model_Monster_Lab_Troll*/
	 PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	 if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Lab_Troll"),
		 CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Lab_Troll/Lab_Troll", PreTransformMatrix))))
		 return E_FAIL;

	 /* For. Prototype_Component_Model_Monster_Lab_Troll_Knife*/
	 PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	 if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Lab_Troll_Knife"),
		 CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Lab_Troll/Lab_Troll_Knife", PreTransformMatrix))))
		 return E_FAIL;
#pragma endregion
#pragma region ITEM
	 /* For. Prototype_Component_Model_Item_DragonHeart */
	 PreTransformMatrix = XMMatrixIdentity();
	 if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Item_DragonHeart"),
		 CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Item/DragonHeart/DragonHeart", PreTransformMatrix))))
		 return E_FAIL;

	 /* For. Prototype_Component_Model_Item_RuneKey */
	 PreTransformMatrix = XMMatrixIdentity();
	 if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Item_RuneKey"),
		 CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Item/RuneKey/RuneKey", PreTransformMatrix))))
		 return E_FAIL;
#pragma endregion
	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));
#pragma region SHADER
	/* For. Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxAnimModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxCubeTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX::Elements, VTXCUBETEX::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxTrailInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxTrailInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTrailInstance.hlsl"), VTXTRAILINSTANCE::Elements, VTXTRAILINSTANCE::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxTrail_TwoPoint_Instance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxTrail_TwoPoint_Instance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTrail_TwoPoint_Instance.hlsl"), VTXTRAIL_TWOPOINT_INSTANCE::Elements, VTXTRAIL_TWOPOINT_INSTANCE::iNumElements))))
		return E_FAIL;
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("콜라이더을(를) 로딩중입니다."));
#pragma region COLLIDER
	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;
#pragma endregion
#pragma region FSM & NAVIGATION
	lstrcpy(m_szLoadingText, TEXT("FSM을(를) 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_FSM"),
		CFSM::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩중입니다."));
	/* For. Prototype_Component_Navigation*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/SaveData/Navigation.dat")))))
		return E_FAIL;
#pragma endregion
	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
#pragma region CAMERA
	/* For. Prototype_GameObject_Camera_Container */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Container"),
		CCamera_Container::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Camera_Shorder */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Shorder"),
		CCamera_Shorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Camera_Shorder */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Interaction"),
		CCamera_Interaction::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region LABYRINTH
	/* For. Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Labyrinth */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Labyrinth"),
		CLabyrinth::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FloorChunk */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FloorChunk"),
		CFloorChunk::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region PUZZLE
	/* For. Prototype_GameObject_PuzzleBase */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PuzzleBase"),
		CPuzzleBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_PuzzlePart */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PuzzlePart"),
		CPuzzlePart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Puzzle_InterColl */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Puzzle_InterColl"),
		CPuzzle_InterColl::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region PLAYER
	/* For. Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Player_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Body"),
		CPlayer_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Player_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Weapon"),
		CPlayer_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Player_Shield */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Shield"),
		CPlayer_Shield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Player_Item */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Item"),
		CPlayer_Item::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion
#pragma region MONSTER
	/* For. Prototype_GameObject_Particle_Monster_Death */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Monster_Death"),
		CParticle_Monster_Death::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region BOSS
	/* For. Prototype_GameObject_Boss_Lab */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Lab"),
		CBoss_Lab::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Boss_Lab_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Lab_Body"),
		CBoss_Lab_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Boss_Lab_Attack */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Lab_Attack"),
		CBoss_Lab_Attack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Boss_Lab_Teleport_Smoke */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Lab_Teleport_Smoke"),
		CBoss_Lab_Teleport_Smoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Boss_Lab_Teleport_Stone */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Lab_Teleport_Stone"),
		CBoss_Lab_Teleport_Stone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion
#pragma region MONSTER_CONSTRUCT
	/* For. Prototype_GameObject_Lab_Construct */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lab_Construct"),
		CLab_Construct::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Lab_Construct_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lab_Construct_Body"),
		CLab_Construct_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Lab_Construct_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lab_Construct_Attack"),
		CLab_Construct_Attack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion
#pragma region MONSTER_DRUM
	/* For. Prototype_GameObject_Lab_Drum */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lab_Drum"),
		CLab_Drum::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Lab_Drum_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lab_Drum_Body"),
		CLab_Drum_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Lab_Drum_Attack */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lab_Drum_Attack"),
		CLab_Drum_Attack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion
#pragma region MONSTER_MAGE
	/* For. Prototype_GameObject_Lab_Mage */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lab_Mage"),
		CLab_Mage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Lab_Mage_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lab_Mage_Body"),
		CLab_Mage_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Lab_Mage_Attack */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lab_Mage_Attack"),
		CLab_Mage_Attack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion
#pragma region MONSTER_TROLL
	/* For. Prototype_GameObject_Lab_Troll */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lab_Troll"),
		CLab_Troll::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Lab_Troll_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lab_Troll_Body"),
		CLab_Troll_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Lab_Troll_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lab_Troll_Weapon"),
		CLab_Troll_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region INTERACTION TELEPORT
	/* For. Prototype_GameObject_Teleport */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Teleport"),
		CTeleport::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Teleport_Container */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Teleport_Container"),
		CTeleport_Container::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region INTERACTION PEDESTAL
	/* For. Prototype_GameObject_Pedestal */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pedestal"),
		CPedestal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Pedestal_Item */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pedestal_Item"),
		CPedestal_Item::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Pedestal_InterColl */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pedestal_InterColl"),
		CPedestal_InterColl::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region INTERACTION WAYPOINT
	/* For. Prototype_GameObject_WayPoint */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WayPoint"),
		CWayPoint::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_WayPoint_InterColl */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WayPoint_InterColl"),
		CWayPoint_InterColl::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region INTERACTION DOORLOCK
	/* For. Prototype_GameObject_DoorLock */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DoorLock"),
		CDoorLock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_DoorLock_InterColl */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DoorLock_InterColl"),
		CDoorLock_InterColl::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_DoorLock_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DoorLock_Effect"),
		CDoorLock_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region PARTICLE
	/* For. Prototype_GameObject_Particle_Spark */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Spark"),
		CParticle_AttackLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Particle_Spawn */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Spawn"),
		CParticle_Spawn::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_Particle_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Snow"),
		CParticle_Snow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Particle_Smoke */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Smoke"),
		CParticle_Smoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Particle_LaunchStone */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_LaunchStone"),
		CParticle_LaunchStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Particle_LaunchWaterDrop */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_LaunchWaterDrop"),
		CParticle_LaunchWaterDrop::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region EFFECT
	/* For. Prototype_GameObject_Effect_Flash */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Flash"),
		CEffect_Flash::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Effect_Spark */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Spark"),
		CEffect_Spark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Effect_BloodCore */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_BloodCore"),
		CEffect_BloodCore::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Effect_BloodSpray */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_BloodSpray"),
		CEffect_BloodSpray::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Effect_Flare */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Flare"),
		CEffect_Flare::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region TRAIL
	/* For. Prototype_GameObject_Trail_Revolve */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trail_Revolve"),
		CTrail_Revolve::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region INVENTORY
	/* For. Prototype_GameObject_Inventory */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Inventory"),
		CInventory::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region ITEM
	/* For. Prototype_GameObject_DragonHeart */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DragonHeart"),
		CDragonHeart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_PuzzleItem */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PuzzleItem"),
		CPuzzle_Item::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_RuneKey */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RuneKey"),
		CRuneKey::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region UI
	/* For. Prototype_GameObject_UI_PlayerBase */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_PlayerBase"),
		CUI_PlayerBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_UI_DragonHeart */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_DragonHeart"),
		CUI_DragonHeart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_UI_DragonStone */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_DragonStone"),
		CUI_DragonStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_UI_GageBar */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_GageBar"),
		CUI_GageBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_UI_MonsterHPBar */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MonsterHPBar"),
		CUI_MonsterHPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_UI_BossHPBar */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BossHPBar"),
		CUI_BossHPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_UI_BossHPBar */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BossHPBar_Deco"),
		CUI_BossHPBarDeco::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_UI_BossHPBar_Glow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BossHPBar_Glow"),
		CUI_BossHPBarGlow::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Particle()
{
	_char ConvergeFilePath[MAX_PATH]{ "../Bin/SaveData/Spawn/Converge_Particle.dat" };
	ifstream infile_Converge(ConvergeFilePath, ios::binary);

	if (!infile_Converge.is_open())
		return E_FAIL;
	while (true)
	{
		CVIBuffer_Instancing::INSTANCE_DESC			ParticleDesc{};

		infile_Converge.read(reinterpret_cast<_char*>(&ParticleDesc), sizeof(CVIBuffer_Instancing::INSTANCE_DESC));

		ParticleDesc.vMinColor = {0.137f, 0.f, 0.204f, 1.f};
		ParticleDesc.vMaxColor = {0.188f, 0.098f, 0.216f, 1.f};
		//ParticleDesc.vSize.x *= 0.5f;
		//ParticleDesc.vSize.y *= 0.5f;
		ParticleDesc.vRange = { 8.f, 8.f, 8.f };
		ParticleDesc.vSpeed.x *= 2.f;
		ParticleDesc.vSpeed.y *= 2.f;
		//ParticleDesc.vMinColor = { 0.541f, 0.f, 0.827f, 1.f };
		//ParticleDesc.vMaxColor = { 0.580f, 0.169f, 0.886f, 1.f };
		// ParticleDesc.iNumInstance = 500;


		if (true == infile_Converge.eof())
			break;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Spawn_Converge"),
			CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
			return E_FAIL;
	}
	infile_Converge.close();

	_char SpreadFilePath[MAX_PATH] = { "../Bin/SaveData/Spawn/Spread_Particle.dat" };
	ifstream infile_Spread(SpreadFilePath, ios::binary);

	if (!infile_Spread.is_open())
		return E_FAIL;
	while (true)
	{
		CVIBuffer_Instancing::INSTANCE_DESC			ParticleDesc{};

		infile_Spread.read(reinterpret_cast<_char*>(&ParticleDesc), sizeof(CVIBuffer_Instancing::INSTANCE_DESC));

		//ParticleDesc.vMinColor = { 0.541f, 0.f, 0.827f, 1.f };
		//ParticleDesc.vMaxColor = { 0.580f, 0.169f, 0.886f, 1.f };
		ParticleDesc.vMinColor = { 0.137f, 0.f, 0.204f, 1.f };
		ParticleDesc.vMaxColor = { 0.188f, 0.098f, 0.216f, 1.f };

		/*ParticleDesc.vSize.x *= 0.5f;
		ParticleDesc.vSize.y *= 0.5f;*/

		ParticleDesc.vSpeed.x *= 2.f;
		ParticleDesc.vSpeed.y *= 2.f;
		ParticleDesc.vLifeTime.x *= 0.5f;
		ParticleDesc.vLifeTime.y *= 0.5f;

		if (true == infile_Spread.eof())
			break;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Spawn_Spread"),
			CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
			return E_FAIL;
	}
	infile_Spread.close();

	return S_OK;
}


CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
