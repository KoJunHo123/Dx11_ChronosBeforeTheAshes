#include "stdafx.h"
#include "UI_PlayerBase.h"
#include "GameInstance.h"

#include "Player.h"

#include "UI_DragonHeart.h"
#include "Inventory.h"
#include "DragonHeart.h"

#include "UI_DragonStone.h"
#include "UI_GageBar.h"
#include "UI_YouDied.h"
#include "UI_Interaction.h"
#include "UI_Puzzle.h"
#include "UI_PuzzleExit.h"

CUI_PlayerBase::CUI_PlayerBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_PlayerBase::CUI_PlayerBase(const CUI_PlayerBase& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CUI_PlayerBase::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_PlayerBase::Initialize(void* pArg)
{
	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0));
	Safe_AddRef(m_pPlayer);


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_UI()))
		return E_FAIL;


	return S_OK;
}

_uint CUI_PlayerBase::Priority_Update(_float fTimeDelta)
{
	for(auto& UI : m_UIObjects)
	{
		if (nullptr == UI)
			continue;
		UI->Priority_Update(fTimeDelta);
	}

	return OBJ_NOEVENT;
}

void CUI_PlayerBase::Update(_float fTimeDelta)
{
	for (auto& UI : m_UIObjects)
	{
		if (nullptr == UI)
			continue;
		UI->Update(fTimeDelta);
	}
}

void CUI_PlayerBase::Late_Update(_float fTimeDelta)
{
	for (auto& UI : m_UIObjects)
	{
		if (nullptr == UI)
			continue;
		UI->Late_Update(fTimeDelta);
	}
}

HRESULT CUI_PlayerBase::Render()
{
	return S_OK;
}

HRESULT CUI_PlayerBase::Ready_UI()
{
	CInventory* pInventory = static_cast<CInventory*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Inventory"), 0));

	CUI_DragonHeart::UI_DRAGONHEART_DESC HeartDesc = {};
	HeartDesc.fRotationPerSec = XMConvertToRadians(90.f);
	HeartDesc.fSpeedPerSec = 1.f;
	HeartDesc.fSizeX = 55.f;
	HeartDesc.fSizeY = 55.f;
	HeartDesc.fX = 116.5f;
	HeartDesc.fY = 97.5f;
	HeartDesc.pDragonHeart = static_cast<CDragonHeart*>(pInventory->Find_Item(TEXT("Item_DragonHeart")));

	m_UIObjects[UI_DRAGONHEART] = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_DragonHeart"), &HeartDesc));
	
	CUI_DragonStone::UI_DRAGONSTONE_DESC StoneDesc = {};
	StoneDesc.fRotationPerSec = XMConvertToRadians(90.f);
	StoneDesc.fSpeedPerSec = 1.f;
	StoneDesc.fSizeX = 64.f;
	StoneDesc.fSizeY = 64.f;
	StoneDesc.fX = 92.f;
	StoneDesc.fY = 69.f;
	StoneDesc.pPlayer = m_pPlayer;

	m_UIObjects[UI_DRAGONSTONE] = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_DragonStone"), &StoneDesc));

	CUI_GageBar::UI_GAGEBAR_DESC BarDesc = {};

	BarDesc.fRotationPerSec = XMConvertToRadians(90.f);
	BarDesc.fSpeedPerSec = 1.f;
	BarDesc.fSizeX = 277.;
	BarDesc.fSizeY = 14.f;
	BarDesc.fX = 252.5f;
	BarDesc.fY = 61.f;
	BarDesc.pPlayer = m_pPlayer;
	BarDesc.eType = CUI_GageBar::TYPE_HP;

	m_UIObjects[UI_HP] = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_GageBar"), &BarDesc));

	BarDesc.fSizeX = 277.;
	BarDesc.fSizeY = 8.f;
	BarDesc.fX = 252.5f;
	BarDesc.fY = 72.f;
	BarDesc.pPlayer = m_pPlayer;
	BarDesc.eType = CUI_GageBar::TYPE_STAMINA;

	m_UIObjects[UI_STAMINA] = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_GageBar"), &BarDesc));

	BarDesc.fSizeX = 277.;
	BarDesc.fSizeY = 8.f;
	BarDesc.fX = 252.5f;
	BarDesc.fY = 80.f;
	BarDesc.pPlayer = m_pPlayer;
	BarDesc.eType = CUI_GageBar::TYPE_SKILL;

	m_UIObjects[UI_SKILL] = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_GageBar"), &BarDesc));

	CUI_YouDied::UI_YOUDIED_DESC DiedDesc = {};
	DiedDesc.fRotationPerSec = XMConvertToRadians(90.f);
	DiedDesc.fSpeedPerSec = 1.f;
	DiedDesc.fSizeX = g_iWinSizeX;
	DiedDesc.fSizeY = g_iWinSizeY;
	DiedDesc.fX = g_iWinSizeX * 0.5f;
	DiedDesc.fY = g_iWinSizeY * 0.5f;
	DiedDesc.pPlayer = m_pPlayer;

	m_UIObjects[UI_YOUDIED] = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_YouDied"), &DiedDesc));

	CUI_Interaction::UI_INTERACTION_DESC InteractionDesc = {};

	InteractionDesc.fRotationPerSec = XMConvertToRadians(90.f);
	InteractionDesc.fSpeedPerSec = 1.f;
	InteractionDesc.fSizeX = 50.f;
	InteractionDesc.fSizeY = 50.f;
	InteractionDesc.fX = 0.f;
	InteractionDesc.fY = 0.f;
	InteractionDesc.pPlayer = m_pPlayer;

	m_UIObjects[UI_INTERACTION] = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Interaction"), &InteractionDesc));

	CUI_Puzzle::UI_PUZZLE_DESC PuzzleDesc = {};

	PuzzleDesc.fRotationPerSec = XMConvertToRadians(90.f);
	PuzzleDesc.fSpeedPerSec = 1.f;
	PuzzleDesc.fSizeX = 80.f;
	PuzzleDesc.fSizeY = 50.f;
	PuzzleDesc.fX = 1030.f;
	PuzzleDesc.fY = 590.f;
	PuzzleDesc.pPlayer = m_pPlayer;
	PuzzleDesc.pInventory = pInventory;

	m_UIObjects[UI_PUZZLE] = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Puzzle"), &PuzzleDesc));
	
	CUI_PuzzleExit::UI_PUZZLEEXIT_DESC PuzzleExitDesc = {};

	PuzzleExitDesc.fRotationPerSec = XMConvertToRadians(90.f);
	PuzzleExitDesc.fSpeedPerSec = 1.f;
	PuzzleExitDesc.fSizeX = 50.f;
	PuzzleExitDesc.fSizeY = 50.f;
	PuzzleExitDesc.fX = 1030.f;
	PuzzleExitDesc.fY = 520.f;
	PuzzleExitDesc.pPlayer = m_pPlayer;

	m_UIObjects[UI_PUZZLEEXIT] = static_cast<CUIObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_PuzzleExit"), &PuzzleExitDesc));

	return S_OK;
}

CUI_PlayerBase* CUI_PlayerBase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PlayerBase* pInstance = new CUI_PlayerBase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CUI_PlayerBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_PlayerBase::Clone(void* pArg)
{
	CUI_PlayerBase* pInstance = new CUI_PlayerBase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CUI_PlayerBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_PlayerBase::Pooling()
{
	return new CUI_PlayerBase(*this);
}

void CUI_PlayerBase::Free()
{
	__super::Free();

	for (auto& UI : m_UIObjects)
		Safe_Release(UI);

	Safe_Release(m_pPlayer);
}
