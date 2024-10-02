#include "stdafx.h"
#include "Player_Jump.h"
#include "GameInstance.h"
#include "PartObject.h"

CPlayer_Jump::CPlayer_Jump()
{
}

HRESULT CPlayer_Jump::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	PLAYER_STATE_JUMP_DESC* pDesc  = static_cast<PLAYER_STATE_JUMP_DESC*>(pArg);

	m_pHP = pDesc->pHP;

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
	*m_pPlayerAnim = PLAYER_JUMP_FALL;

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_fSpeed += fTimeDelta;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPos, XMVectorGetY(vPos) - m_fSpeed));

	for (auto& pPartObject : m_Parts)
	{
		if (nullptr == pPartObject)
			continue;
		pPartObject->Update(fTimeDelta);
	}


	if (XMVectorGetY(vPos) < -20.f)
	{
		*m_pHP = 0;
		m_fSpeed = 0.f;
	}
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
