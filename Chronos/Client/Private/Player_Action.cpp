#include "stdafx.h"
#include "Player_Action.h"

CPlayer_Action::CPlayer_Action()
{
}

HRESULT CPlayer_Action::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

HRESULT CPlayer_Action::StartState(void** pArg)
{
	__super::StartState(pArg);

	return S_OK;
}

void CPlayer_Action::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayer_Action::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CPlayer_Action::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

}

HRESULT CPlayer_Action::Render()
{
	__super::Render();
	return S_OK;
}

HRESULT CPlayer_Action::ExitState(void** pArg)
{
	__super::ExitState(pArg);

	return S_OK;
}

CPlayer_Action* CPlayer_Action::Create(void* pArg)
{
	CPlayer_Action* pInstance = new CPlayer_Action();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_Action"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Action::Free()
{
	__super::Free();
}
