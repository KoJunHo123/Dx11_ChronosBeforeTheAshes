#include "stdafx.h"
#include "Player_Attack.h"
#include "GameInstance.h"

#include "Player.h"

#include "Player_Body.h"

CPlayer_Attack::CPlayer_Attack()
{
}

HRESULT CPlayer_Attack::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

HRESULT CPlayer_Attack::StartState(void** pArg)
{
	__super::StartState(pArg);

	return S_OK;
}

void CPlayer_Attack::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayer_Attack::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if(5 > static_cast<CPlayer_Body*>(m_Parts[CPlayer::PART_BODY])->Get_FrameIndex())
		Look_CameraDir();

	if(false == m_bMotionLock)
	{
		if (PLAYER_MOVE_DODGE_F == *m_pPlayerAnim && m_pGameInstance->Key_Up(VK_LBUTTON))
		{
			m_eAttackState = ATTACK_RUN;
			*m_pPlayerAnim = PLAYER_ATK_RUN;
			m_bMotionLock = true;
		}
		else if(m_pGameInstance->Key_Up(VK_LBUTTON))
		{
			m_fAttackDelay = 0.f;
			if (m_fAttackDelay < 0.2f)
				Light_Attack();
			else
				Power_Attack();
		}
		else if (m_pGameInstance->Key_Pressing(VK_LBUTTON) && m_pGameInstance->Key_Pressing(VK_SHIFT))
		{
			m_fAttackDelay += fTimeDelta;

			if (0 == m_iAttackCount && m_fAttackDelay)
			{
				m_eAttackState = ATTACK_POWER_CHARGE;
				*m_pPlayerAnim = PLAYER_ATK_POWER_01_CHARGE;
			}
			else if (1 == m_iAttackCount && m_fAttackDelay)
			{
				m_eAttackState = ATTACK_POWER_CHARGE;
				*m_pPlayerAnim = PLAYER_ATK_POWER_02_CHARGE;
			}

			if (true == *m_pIsFinished)
			{
				Power_Attack();
				*m_pIsFinished = false;
			}
		}
		else if(20 < static_cast<CPlayer_Body*>(m_Parts[CPlayer::PART_BODY])->Get_FrameIndex())
		{
			if (m_pGameInstance->Key_Pressing('W')
				|| m_pGameInstance->Key_Pressing('A')
				|| m_pGameInstance->Key_Pressing('S')
				|| m_pGameInstance->Key_Pressing('D')
				|| m_pGameInstance->Key_Down(VK_SPACE))
				m_pFSM->Set_State(CPlayer::STATE_MOVE);

			if (m_pGameInstance->Key_Pressing(VK_RBUTTON) && true == *m_pIsFinished)
				m_pFSM->Set_State(CPlayer::STATE_BLOCK);
		}
	}

	if (true == *m_pIsFinished)
	{
		m_pFSM->Set_State(CPlayer::STATE_MOVE);
	}
	
	if ((ATTACK_POWER == m_eAttackState && m_iAttackCount >= m_iMaxPower)
		|| (ATTACK_LIGHT == m_eAttackState && m_iAttackCount >= m_iMaxLight))
		m_iAttackCount = 0;


	if (ATTACK_POWER_CHARGE != m_eAttackState)
	{
		if (true == *m_pIsFinished)
		{
			m_pFSM->Set_State(CPlayer::STATE_MOVE);
		}

		_uint KeyFrameIndex = static_cast<CPlayer_Body*>(m_Parts[CPlayer::PART_BODY])->Get_FrameIndex();
		_uint CurrentAnimIndex = static_cast<CPlayer_Body*>(m_Parts[CPlayer::PART_BODY])->Get_CurrentAnimIndex();

		if (15 < KeyFrameIndex && *m_pPlayerAnim == CurrentAnimIndex)
			m_bMotionLock = false;
	}

}

void CPlayer_Attack::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CPlayer_Attack::Render()
{
	__super::Render();
	return S_OK;
}

HRESULT CPlayer_Attack::ExitState(void** pArg)
{
	__super::ExitState(pArg);
	m_iAttackCount = 0;
	m_eAttackState = ATTACK_END;
	return S_OK;
}

void CPlayer_Attack::Light_Attack()
{
	m_eAttackState = ATTACK_LIGHT;
	switch (m_iAttackCount)
	{
	case 0:
		*m_pPlayerAnim = PLAYER_ATK_LIGHT_01;
		m_bMotionLock = true;
		m_fAttackDelay = 0.f;
		break;
	case 1:
		*m_pPlayerAnim = PLAYER_ATK_LIGHT_02;
		m_bMotionLock = true;
		m_fAttackDelay = 0.f;
		break;
	case 2:
		*m_pPlayerAnim = PLAYER_ATK_LIGHT_03;
		m_bMotionLock = true;
		m_fAttackDelay = 0.f;
		break;
	case 3:
		*m_pPlayerAnim = PLAYER_ATK_LIGHT_04;
		m_bMotionLock = true;
		m_fAttackDelay = 0.f;
		break;
	default:
		break;
	}

	++m_iAttackCount;
}

void CPlayer_Attack::Power_Attack()
{
	m_eAttackState = ATTACK_POWER;
	switch (m_iAttackCount)
	{
	case 0:
		*m_pPlayerAnim = PLAYER_ATK_POWER_01;
		m_bMotionLock = true;
		m_fAttackDelay = 0.f;
		break;
	case 1:
		*m_pPlayerAnim = PLAYER_ATK_POWER_02;
		m_bMotionLock = true;
		m_fAttackDelay = 0.f;
		break;
	default:
		break;
	}

	++m_iAttackCount;
}

CPlayer_Attack* CPlayer_Attack::Create(void* pArg)
{
	CPlayer_Attack* pInstance = new CPlayer_Attack();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Attack::Free()
{
	__super::Free();
}
