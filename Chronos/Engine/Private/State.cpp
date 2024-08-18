#include "State.h"
#include "GameInstance.h"

CState::CState()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
}

HRESULT CState::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	STATE_DESC* pDesc = static_cast<STATE_DESC*>(pArg);

	m_pFSM = pDesc->pFSM;

	return S_OK;
}

HRESULT CState::StartState(void** pArg)
{
	return S_OK;
}

void CState::Priority_Update(_float fTimeDelta)
{
}

void CState::Update(_float fTimeDelta)
{
}

void CState::Late_Update(_float fTimeDelta)
{
}

HRESULT CState::Render()
{
	return S_OK;
}

HRESULT CState::ExitState(void** pArg)
{
	return S_OK;
}

void CState::Free()
{
	__super::Free();
}
