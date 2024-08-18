#include "stdafx.h"
#include "Player_Impact.h"

CPlayer_Impact::CPlayer_Impact()
{
}

HRESULT CPlayer_Impact::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

HRESULT CPlayer_Impact::StartState(void** pArg)
{
	__super::StartState(pArg);

	return S_OK;
}

void CPlayer_Impact::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayer_Impact::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

}

void CPlayer_Impact::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);


}

HRESULT CPlayer_Impact::Render()
{
	__super::Render();
	return S_OK;
}

HRESULT CPlayer_Impact::ExitState(void** pArg)
{
	__super::ExitState(pArg);

	return S_OK;
}

CPlayer_Impact* CPlayer_Impact::Create(void* pArg)
{
	CPlayer_Impact* pInstance = new CPlayer_Impact();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_Impact"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Impact::Free()
{
	__super::Free();
}
