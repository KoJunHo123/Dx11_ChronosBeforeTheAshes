#include "stdafx.h"
#include "Player_Action.h"
#include "GameInstance.h"

#include "Player.h"
#include "Player_Item.h"

CPlayer_Action::CPlayer_Action()
{
}

HRESULT CPlayer_Action::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	PLAYER_ACTION_DESC* pDesc = static_cast<PLAYER_ACTION_DESC*>(pArg);

	m_pPlayerColliderCom = pDesc->pPlayerColliderCom;
	m_pItemUsed = pDesc->pItemUsed;

	Safe_AddRef(m_pPlayerColliderCom);

	return S_OK;
}

HRESULT CPlayer_Action::StartState(void** pArg)
{
	__super::StartState(pArg);

	if(STATE_DRAGONHEART != m_eState && STATE_DRAGONSTONE != m_eState)
		m_pPlayerColliderCom->Set_OnCollision(false);

	return S_OK;
}

void CPlayer_Action::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayer_Action::Update(_float fTimeDelta)
{
	if (STATE_DRAGONHEART == m_eState)
		*m_pPlayerAnim = PLAYER_ACTION_DRAGONHEART;
	else if (STATE_DRAGONSTONE == m_eState)
		*m_pPlayerAnim = PLAYER_ACTION_DRAGONSTONE;
	

	if (true == *m_pIsFinished)
	{
		m_pFSM->Set_State(CPlayer::STATE_MOVE);
	}

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

	*m_pItemUsed = false;
	static_cast<CPlayer_Item*>(m_Parts[CPlayer::PART_ITEM])->Release_Item();

	m_pPlayerColliderCom->Set_OnCollision(true);

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

	Safe_Release(m_pPlayerColliderCom);
}
