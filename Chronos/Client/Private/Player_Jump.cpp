#include "stdafx.h"
#include "Player_Jump.h"

CPlayer_Jump::CPlayer_Jump()
{
}

HRESULT CPlayer_Jump::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

HRESULT CPlayer_Jump::StartState(void** pArg)
{
	__super::StartState(pArg);

	return S_OK;
}

void CPlayer_Jump::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayer_Jump::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CPlayer_Jump::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

}

HRESULT CPlayer_Jump::Render()
{
	__super::Render();
	return S_OK;
}

HRESULT CPlayer_Jump::ExitState(void** pArg)
{
	__super::ExitState(pArg);

	return S_OK;
}

CPlayer_Jump* CPlayer_Jump::Create(void* pArg)
{
	CPlayer_Jump* pInstance = new CPlayer_Jump();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_Jump"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Jump::Free()
{
	__super::Free();
}
