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

	PLAYER_STATE_MOVE_DESC* pDesc = static_cast<PLAYER_STATE_MOVE_DESC*>(pArg);

	m_pNonIntersect = pDesc->pNonIntersect;
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
	if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT))
	{
		Jog(fTimeDelta);
	}
	else
	{
		Walk(fTimeDelta);
		m_fDodgeDelay = 0.f;
	}

	if (m_pGameInstance->Get_DIKeyState_Down(DIK_SPACE))
	{
		Dodge();
	}

}

void CPlayer_Move::Walk(_float fTimeDelta)
{
	_bool bKeyPress = { false };

	if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_W))
	{
		*m_pPlayerAnim = PLAYER_MOVE_WALK_F;
		m_eMoveState = MOVE_WALK;

		Look_CameraDir();
		m_pTransformCom->Go_Straight(fTimeDelta * *m_pSpeed, m_pNavigationCom);

		bKeyPress = true;
	}
	if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_S))
	{
		*m_pPlayerAnim = PLAYER_MOVE_WALK_B;
		m_eMoveState = MOVE_WALK;

		Look_CameraDir();
		m_pTransformCom->Go_Backward(fTimeDelta * *m_pSpeed, m_pNavigationCom);

		bKeyPress = true;
	}
	if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_A))
	{
		*m_pPlayerAnim = PLAYER_MOVE_WALK_L;		
		m_eMoveState = MOVE_WALK;

		Look_CameraDir();
		m_pTransformCom->Go_Left(fTimeDelta * *m_pSpeed, m_pNavigationCom);

		bKeyPress = true;
	}
	if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_D))
	{
		*m_pPlayerAnim = PLAYER_MOVE_WALK_R;
		m_eMoveState = MOVE_WALK;
		
		Look_CameraDir();
		m_pTransformCom->Go_Right(fTimeDelta * *m_pSpeed, m_pNavigationCom);

		bKeyPress = true;
	}
	if(false == bKeyPress)
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
	_bool bKeyPress = { false };
	if(m_pGameInstance->Get_DIKeyState(DIKEYBOARD_W))
	{
 		*m_pPlayerAnim = PLAYER_MOVE_JOG_F;
		m_eMoveState = MOVE_JOG;
		
		Look_CameraDir();
		m_pTransformCom->Go_Straight(fTimeDelta * (*m_pSpeed * 2.f) * 1.f, m_pNavigationCom);

		bKeyPress = true;
	}
	if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_S))
	{
		*m_pPlayerAnim = PLAYER_MOVE_JOG_B;
		m_eMoveState = MOVE_JOG;

		Look_CameraDir();
		m_pTransformCom->Go_Backward(fTimeDelta * (*m_pSpeed * 2.f) * 1.f, m_pNavigationCom);

		bKeyPress = true;
	}
	if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_A))
	{
		*m_pPlayerAnim = PLAYER_MOVE_JOG_L;
		m_eMoveState = MOVE_JOG;

		Look_CameraDir();
		m_pTransformCom->Go_Left(fTimeDelta * (*m_pSpeed * 2.f) * 1.f, m_pNavigationCom);

		bKeyPress = true;
	}
	if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_D))
	{
		*m_pPlayerAnim = PLAYER_MOVE_JOG_R;
		m_eMoveState = MOVE_JOG;

		Look_CameraDir();
		m_pTransformCom->Go_Right(fTimeDelta * (*m_pSpeed * 2.f) * 1.f, m_pNavigationCom);

		bKeyPress = true;
	}
	if(false == bKeyPress)
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
	if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_A))
	{
		*m_pPlayerAnim = PLAYER_MOVE_DODGE_L;
		m_bMotionLock = true;
		*m_pNonIntersect = true;
	}
	else if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_S))
	{
		*m_pPlayerAnim = PLAYER_MOVE_DODGE_B;
		m_bMotionLock = true;
		*m_pNonIntersect = true;
	}
	else if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_D))
	{
		*m_pPlayerAnim = PLAYER_MOVE_DODGE_R;
		m_bMotionLock = true;
		*m_pNonIntersect = true;
	}
	else
	{
		*m_pPlayerAnim = PLAYER_MOVE_DODGE_F;
		m_bMotionLock = true;
		*m_pNonIntersect = true;
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

		if (10 < KeyFrameIndex)
		{
			*m_pNonIntersect = false;
			if (m_pGameInstance->Get_DIMouseState_Down(DIMK_LBUTTON))
				m_pFSM->Set_State(CPlayer::STATE_ATTACK);
		}
		if (15 < KeyFrameIndex)
		{
			m_bMotionLock = false;
			m_fDodgeDelay = 0.f;
		}
		
	}
}

void CPlayer_Move::Change_State()
{
	if (m_pGameInstance->Get_DIMouseState_Down(DIMK_LBUTTON))
		m_pFSM->Set_State(CPlayer::STATE_ATTACK);
	else if (m_pGameInstance->Get_DIMouseState(DIMK_RBUTTON))
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
