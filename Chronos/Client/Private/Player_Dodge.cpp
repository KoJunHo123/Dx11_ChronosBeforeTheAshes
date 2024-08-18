#include "stdafx.h"
#include "Player_Dodge.h"

#include "GameInstance.h"
#include "Player.h"

CPlayer_Dodge::CPlayer_Dodge()
{
}

HRESULT CPlayer_Dodge::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

HRESULT CPlayer_Dodge::StartState()
{
	__super::StartState();

	return S_OK;
}

void CPlayer_Dodge::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayer_Dodge::Update(_float fTimeDelta)
{

	__super::Update(fTimeDelta);
}

void CPlayer_Dodge::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CPlayer_Dodge::Render()
{

	__super::Render();
	return S_OK;
}

HRESULT CPlayer_Dodge::ExitState()
{
	__super::ExitState();

	return S_OK;
}

CPlayer_Dodge* CPlayer_Dodge::Create(void* pArg)
{
	CPlayer_Dodge* pInstance = new CPlayer_Dodge();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_Dodge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Dodge::Free()
{
	__super::Free();
}
