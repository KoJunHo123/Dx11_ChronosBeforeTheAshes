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
	else if (STATE_WAYPOINT == m_eState)
	{
		if(PLAYER_ACTION_WAYPOINT_RELEASE != *m_pPlayerAnim)
		{
			*m_pPlayerAnim = PLAYER_ACTION_WAYPOINT_GRAB;
		}
	}
	else if (STATE_RUNEKEY == m_eState)
	{
		*m_pPlayerAnim = PLAYER_ACTION_RUNEKEYHOLE;
	}
	else if (STATE_TELEPORT == m_eState)
	{
		if(false == m_bTeleportStart)
		{
			*m_pPlayerAnim = PLAYER_ACTION_TELEPORT_LONG_ENTER;
			m_bTeleportStart = true;
		}

		if (PLAYER_ACTION_TELEPORT_LONG_IDLE == *m_pPlayerAnim)
		{
			if (false == m_bFadeIn)
			{
				m_bFadeIn = m_pGameInstance->FadeIn(fTimeDelta);
				if (true == m_bFadeIn)
				{
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vTargetPosition));
					m_pNavigationCom->Set_CurrentCellIndex_ByPos(XMLoadFloat3(&m_vTargetPosition));
				}
			}
			else
			{
				m_bFadeOut = m_pGameInstance->FadeOut(fTimeDelta);
				if (true == m_bFadeOut)
					*m_pPlayerAnim = PLAYER_ACTION_TELEPORT_LONG_EXIT;
			}
		}
	}

	if (true == *m_pIsFinished)
	{
		if (PLAYER_ACTION_WAYPOINT_GRAB == *m_pPlayerAnim)
			*m_pPlayerAnim = PLAYER_ACTION_WAYPOINT_RELEASE;
		else if(PLAYER_ACTION_TELEPORT_LONG_ENTER == *m_pPlayerAnim)
			*m_pPlayerAnim = PLAYER_ACTION_TELEPORT_LONG_IDLE;
		else
			m_pFSM->Set_State(CPlayer::STATE_MOVE);

		if (PLAYER_ACTION_TELEPORT_LONG_EXIT == *m_pPlayerAnim)
		{
			m_bFadeIn = false;
			m_bFadeOut = false;
		}

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
	m_bTeleportStart = false;
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
