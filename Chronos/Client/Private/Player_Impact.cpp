#include "stdafx.h"
#include "Player_Impact.h"
#include "GameInstance.h"
#include "Player.h"

CPlayer_Impact::CPlayer_Impact()
{
}

HRESULT CPlayer_Impact::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	PLAYER_STATE_IMPACT_DESC* pDesc = static_cast<PLAYER_STATE_IMPACT_DESC*>(pArg);

	m_pHP = pDesc->pHP;

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
	if(*m_pPlayerAnim != PLAYER_IMPACT_DEATH_F && *m_pPlayerAnim != PLAYER_IMPACT_DEATH_B)
	{
		if (90 > abs(m_fHittedAngle))
		{
			if (*m_pHP <= 0)
				*m_pPlayerAnim = PLAYER_IMPACT_DEATH_F;
			else
				*m_pPlayerAnim = PLAYER_IMPACT_HEAVY_FROMF;
		}
		else
		{
			if (*m_pHP <= 0)
				*m_pPlayerAnim = PLAYER_IMPACT_DEATH_B;
			else
				*m_pPlayerAnim = PLAYER_IMPACT_HEAVY_FROMB;
		}
	}

	if (true == *m_pIsFinished && 0 < *m_pHP)
	{
		*m_pIsFinished = false;
		m_pFSM->Set_State(CPlayer::STATE_MOVE);
	}

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
