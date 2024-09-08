#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"

// 오브젝트

// 지형
#include "BackGround.h"
#include "Labyrinth.h"
#include "Terrain.h"
#include "FloorChunk.h"
#include "Sky.h"

// 퍼즐
#include "PuzzleBase.h"
#include "PuzzlePart.h"

// 카메라
#include "FreeCamera.h"

// 플레이어
#include "Player.h"
#include "Player_Body.h"
#include "Player_Weapon.h"
#include "Player_Shield.h"

// 보스몬스터
#include "Boss_Lab.h"
#include "Boss_Lab_Body.h"
#include "Boss_Lab_Attack.h"

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
	SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Ready_Resources_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	/* For. Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo.png"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	/* For. Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	/* For. Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_1.dds"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));
#pragma region VIBUFFER
	/* For. Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
#pragma region LABYRINTH
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	/* For. Prototype_Component_Model_Labyrinth*/
	// PreTransformMatrix = XMMatrixIdentity();
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Labyrinth"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Labyrinth/Labyrinth/Labyrinth", PreTransformMatrix))))
	//	return E_FAIL;

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
	/* For. Prototype_GameObject_FreeCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
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
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

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
