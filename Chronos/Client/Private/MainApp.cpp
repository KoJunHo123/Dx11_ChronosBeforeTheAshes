#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "Level_Loading.h"
#include "GameInstance.h"

#include "BackGround.h"
#include "UI_LoadingMaze.h"

CMainApp::CMainApp()
	: m_pGameInstance { CGameInstance::Get_Instance() }
{
	/* 내가 레퍼런스 카운트를 증가시키는 시점 : */
	/* 원래 보관하기위한 포인터에 저장을 할때는 증가시키지 않는다. */
	/* 원래 보관된 위치에 있던 주소를 다른 포인터형변수에 저장을 하게되면 이때는 증가시킨다. */
	Safe_AddRef(m_pGameInstance);	
}

HRESULT CMainApp::Initialize()
{	
	ENGINE_DESC			EngineDesc{};
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.isWindowsed = true;

	/* 엔진을 사용하기위한 여러 초기화 과정을 수행한다. */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	/*m_pContext->RSSetState();
	m_pContext->OMSetDepthStencilState();

	D3D11_BLEND_DESC		bLENDdEESC;
	ID3D11BlendState*		pBlendState;
	m_pDevice->CreateBlendState();
	m_pContext->OMSetBlendState(pBlendState);*/

	if (FAILED(Ready_Prototype_Component_Static()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject_Static()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

#ifdef _DEBUG

	if (::AllocConsole() == TRUE)
	{
		FILE* nfp[3];
		freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
		freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
		freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
		std::ios::sync_with_stdio();
	}

#endif // _DEBUG

	//if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_145"), TEXT("../Bin/Resources/Fonts/143ex.spritefont"))))
	//	return E_FAIL;


	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);

	if (FAILED(FrameOut(fTimeDelta)))
		return;
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Render_Begin();

	m_pGameInstance->Draw_Engine();

	///* MakeSpriteFont "넥슨lv1고딕 Bold" /FontSize:10 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 145ex.spritefont */
	//m_pGameInstance->Render_Text(TEXT("Font_145"), TEXT("담배 하나 피자."), XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pGameInstance->Render_End();

	return S_OK;
}


HRESULT CMainApp::Ready_Prototype_Component_Static()
{
	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;	

	///* For.Prototype_Component_Shader_VtxNorTex */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	//	return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_BackGround_Black */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BackGround_Black"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Black.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Logo_Maze */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Logo_Maze"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Logo_Maze.png"), 1))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject_Static()
{
#pragma region BACKGROUND
	/* For. Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_UI_LoadingMaze */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LoadingMaze"),
		CUI_LoadingMaze::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eStartLevelID)
{	
	/* 어떤 레벨을 선택하던 로딩 레벨로 선 진입한다.  */
	/* 로딩레벨에서 내가 선택한 레벨에 필요한 자원을 준비한다. */
	/* 로딩 레벨은 다음레벨(내가 선택한 레벨)이 누구인지를 인지해야한다. */
	m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOADING, eStartLevelID));

	return S_OK;
}

HRESULT CMainApp::FrameOut(_float fTimeDelta)
{
	++m_iFPS;

	m_fFrameTime += fTimeDelta;

	if (m_fFrameTime > 1.f)
	{
		swprintf_s(m_szFPS, L"FPS : %d", m_iFPS);
		SetWindowText(g_hWnd, m_szFPS);

		m_iFPS = 0;
		m_fFrameTime = 0.f;
	}

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
	
}

