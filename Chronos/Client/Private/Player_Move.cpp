#include "stdafx.h"
#include "Player_Move.h"
#include "GameInstance.h"

#include "Player.h"
#include "Player_Body.h"

CPlayer_Move::CPlayer_Move()
{
}

HRESULT CPlayer_Move::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

HRESULT CPlayer_Move::StartState(void** pArg)
{
	__super::StartState(pArg);

	return S_OK;
}

void CPlayer_Move::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayer_Move::Update(_float fTimeDelta)
{

	if(false == m_bMotionLock)
	{
		Move_Control(fTimeDelta);
		Change_State();
	}

	__super::Update(fTimeDelta);

	Dodge_Control();
}

void CPlayer_Move::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CPlayer_Move::Render()
{
	__super::Render();
	return S_OK;
}

HRESULT CPlayer_Move::ExitState(void** pArg)
{
	__super::ExitState(pArg);

	m_eMoveState = MOVE_END;

	return S_OK;
}

void CPlayer_Move::Move_Control(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(VK_SHIFT))
	{
		Jog(fTimeDelta);
	}
	else
	{
		Walk(fTimeDelta);
		m_fDodgeDelay = 0.f;
	}

	if (m_pGameInstance->Key_Down(VK_SPACE))
	{
		Dodge();
	}

}

void CPlayer_Move::Walk(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing('W'))
	{
		*m_pPlayerAnim = PLAYER_MOVE_WALK_F;
		m_eMoveState = MOVE_WALK;

		m_pTransformCom->Go_Straight(fTimeDelta * *m_pSpeed);

	}
	else if (m_pGameInstance->Key_Pressing('A'))
	{
		*m_pPlayerAnim = PLAYER_MOVE_WALK_F;		
		m_eMoveState = MOVE_WALK;

		m_pTransformCom->Go_Left(fTimeDelta * *m_pSpeed);

	}
	else if (m_pGameInstance->Key_Pressing('S'))
	{
		*m_pPlayerAnim = PLAYER_MOVE_WALK_B;
		m_eMoveState = MOVE_WALK;
		
		m_pTransformCom->Go_Backward(fTimeDelta * *m_pSpeed);

	}
	else if (m_pGameInstance->Key_Pressing('D'))
	{
		*m_pPlayerAnim = PLAYER_MOVE_WALK_R;
		m_eMoveState = MOVE_WALK;
		
		m_pTransformCom->Go_Right(fTimeDelta * *m_pSpeed);

	}
	else
	{
		if(MOVE_DODGE != m_eMoveState)
		{
			*m_pPlayerAnim = PLAYER_MOVE_IDLE;
			m_eMoveState = MOVE_IDLE;
		}
	}
}

void CPlayer_Move::Jog(_float fTimeDelta)
{
	if(m_pGameInstance->Key_Pressing('W'))
	{
		*m_pPlayerAnim = PLAYER_MOVE_JOG_F;
		m_eMoveState = MOVE_JOG;
		
		m_pTransformCom->Go_Straight(fTimeDelta * (*m_pSpeed * 2.f) * 1.f);

	}
	else if (m_pGameInstance->Key_Pressing('A'))
	{
		*m_pPlayerAnim = PLAYER_MOVE_JOG_L;
		m_eMoveState = MOVE_JOG;

		m_pTransformCom->Go_Left(fTimeDelta * (*m_pSpeed * 2.f) * 1.f);

	}
	else if (m_pGameInstance->Key_Pressing('S'))
	{
		*m_pPlayerAnim = PLAYER_MOVE_JOG_B;
		m_eMoveState = MOVE_JOG;

		m_pTransformCom->Go_Backward(fTimeDelta * (*m_pSpeed * 2.f) * 1.f);

	}
	else if (m_pGameInstance->Key_Pressing('D'))
	{
		*m_pPlayerAnim = PLAYER_MOVE_JOG_R;
		m_eMoveState = MOVE_JOG;

		m_pTransformCom->Go_Right(fTimeDelta * (*m_pSpeed * 2.f) * 1.f);
	}
	else
	{
		if(MOVE_DODGE != m_eMoveState)
		{
			*m_pPlayerAnim = PLAYER_MOVE_IDLE;
			m_eMoveState = MOVE_IDLE;
		}
	}

}

void CPlayer_Move::Dodge()
{
	if (m_pGameInstance->Key_Pressing('A'))
	{
		*m_pPlayerAnim = PLAYER_MOVE_DODGE_L;
		m_bMotionLock = true;
	}
	else if (m_pGameInstance->Key_Pressing('S'))
	{
		*m_pPlayerAnim = PLAYER_MOVE_DODGE_B;
		m_bMotionLock = true;
	}
	else if (m_pGameInstance->Key_Pressing('D'))
	{
		*m_pPlayerAnim = PLAYER_MOVE_DODGE_R;
		m_bMotionLock = true;
	}
	else
	{
		*m_pPlayerAnim = PLAYER_MOVE_DODGE_F;
		m_bMotionLock = true;
	}
	m_eMoveState = MOVE_DODGE;
}

void CPlayer_Move::Dodge_Control()
{
	if (MOVE_DODGE == m_eMoveState)
	{
		if (true == *m_pIsFinished)
		{
			*m_pPlayerAnim = PLAYER_MOVE_IDLE;
			m_eMoveState = MOVE_IDLE;
		}

		_uint KeyFrameIndex = static_cast<CPlayer_Body*>(m_Parts[CPlayer::PART_BODY])->Get_FrameIndex();

		if (15 < KeyFrameIndex)
		{
			m_bMotionLock = false;
			m_fDodgeDelay = 0.f;
		}
	}
}

void CPlayer_Move::Change_State()
{
	if (m_pGameInstance->Key_Down(VK_LBUTTON))
		m_pFSM->Set_State(CPlayer::STATE_ATTACK);
	else if (m_pGameInstance->Key_Pressing(VK_RBUTTON))
		m_pFSM->Set_State(CPlayer::STATE_BLOCK);
}

CPlayer_Move* CPlayer_Move::Create(void* pArg)
{
	CPlayer_Move* pInstance = new CPlayer_Move();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_Move"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Move::Free()
{
	__super::Free();
}
