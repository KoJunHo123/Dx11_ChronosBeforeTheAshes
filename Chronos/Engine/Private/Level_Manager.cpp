#include "..\Public\Level_Manager.h"

#include "GameInstance.h"
#include "Level.h"


CLevel_Manager::CLevel_Manager()
	: m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Initialize()
{
	return S_OK;
}

void CLevel_Manager::Update(_float fTimeDelta)
{
	if (nullptr != m_pCurrentLevel)
		m_pCurrentLevel->Update(fTimeDelta);

	if (m_pNextLevel->Get_LevelIndex() != m_pCurrentLevel->Get_LevelIndex())
	{
		/* 기존 레벨용 자원(객체)을 삭제한다. */
		m_pGameInstance->Clear(m_pCurrentLevel->Get_LevelIndex());

		Safe_Release(m_pCurrentLevel);

		m_pCurrentLevel = m_pNextLevel;
	}
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr != m_pCurrentLevel)
		m_pCurrentLevel->Render();

	return S_OK;
}

void CLevel_Manager::Change_Level(CLevel* pNextLevel)
{
	if (nullptr == m_pCurrentLevel)
		m_pCurrentLevel = pNextLevel;
	m_pNextLevel = pNextLevel;
}

CLevel_Manager * CLevel_Manager::Create()
{
	CLevel_Manager*		pInstance = new CLevel_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pCurrentLevel);
}

