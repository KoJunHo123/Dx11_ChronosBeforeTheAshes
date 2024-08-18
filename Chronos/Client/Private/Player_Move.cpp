#include "stdafx.h"
#include "Player_Move.h"
#include "GameInstance.h"
#include "Player.h"

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

	if (PLAYER_ATK_RUN == m_AnimDesc.eCurrentPlayerAnim)
	{
		SetUp_Animation(PLAYER_MOVE_IDLE);
		m_eMoveState = MOVE_IDLE;
	}

	return S_OK;
}

void CPlayer_Move::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayer_Move::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if(false == m_bMotionLock)
	{
		Move_Control(fTimeDelta);
		Change_State();
	}

	if(false == IsChanging())
	{
		Play_Animation(fTimeDelta);
	}
	else
	{
		Change_Animation(fTimeDelta);
		Move_In_Change(fTimeDelta);
	}

	_vector Test1 = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector Test2 = XMLoadFloat4(&m_vTest);

	if (XMVector3Length(Test1 - Test2).m128_f32[0] > 5)
		int a = 0;

	XMStoreFloat4(&m_vTest, m_pTransformCom->Get_State(CTransform::STATE_POSITION));


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
	if (m_pGameInstance->Key_Up(VK_SPACE) && m_fDodgeDelay < 0.2f)
	{
		if(MOVE_DODGE != m_eMoveState)
			Dodge();
	}
	else if (m_pGameInstance->Key_Pressing(VK_SPACE))
	{
		m_fDodgeDelay += fTimeDelta;	
		if(m_fDodgeDelay > 0.5f)
		{
			Jog(fTimeDelta);
		}
	}
	else
	{
		Walk(fTimeDelta);
		m_fDodgeDelay = 0.f;
	}

}

void CPlayer_Move::Walk(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing('W'))
	{
		SetUp_Animation(PLAYER_MOVE_WALK_F);
		m_eMoveState = MOVE_WALK;

		m_pTransformCom->Go_Straight(fTimeDelta * *m_pSpeed);

	}
	else if (m_pGameInstance->Key_Pressing('A'))
	{
		SetUp_Animation(PLAYER_MOVE_WALK_L);
		m_eMoveState = MOVE_WALK;

		m_pTransformCom->Go_Left(fTimeDelta * *m_pSpeed);

	}
	else if (m_pGameInstance->Key_Pressing('S'))
	{
		SetUp_Animation(PLAYER_MOVE_WALK_B);
		m_eMoveState = MOVE_WALK;
		
		m_pTransformCom->Go_Backward(fTimeDelta * *m_pSpeed);

	}
	else if (m_pGameInstance->Key_Pressing('D'))
	{
		SetUp_Animation(PLAYER_MOVE_WALK_R);
		m_eMoveState = MOVE_WALK;
		
		m_pTransformCom->Go_Right(fTimeDelta * *m_pSpeed);

	}
	else
	{
		if(MOVE_DODGE != m_eMoveState)
		{
			SetUp_Animation(PLAYER_MOVE_IDLE, true);
			m_eMoveState = MOVE_IDLE;
		}
	}
}

void CPlayer_Move::Jog(_float fTimeDelta)
{
	if(m_pGameInstance->Key_Pressing('W'))
	{
		SetUp_Animation(PLAYER_MOVE_JOG_F);
		m_eMoveState = MOVE_JOG;
		
		m_pTransformCom->Go_Straight(fTimeDelta * (*m_pSpeed * 2.f) * 1.f);

	}
	else if (m_pGameInstance->Key_Pressing('A'))
	{
		SetUp_Animation(PLAYER_MOVE_JOG_L);
		m_eMoveState = MOVE_JOG;

		m_pTransformCom->Go_Left(fTimeDelta * (*m_pSpeed * 2.f) * 1.f);

	}
	else if (m_pGameInstance->Key_Pressing('S'))
	{
		SetUp_Animation(PLAYER_MOVE_JOG_B);
		m_eMoveState = MOVE_JOG;

		m_pTransformCom->Go_Backward(fTimeDelta * (*m_pSpeed * 2.f) * 1.f);

	}
	else if (m_pGameInstance->Key_Pressing('D'))
	{
		SetUp_Animation(PLAYER_MOVE_JOG_R);
		m_eMoveState = MOVE_JOG;

		m_pTransformCom->Go_Right(fTimeDelta * (*m_pSpeed * 2.f) * 1.f);
	}
	else
	{
		if(MOVE_DODGE != m_eMoveState)
		{
			SetUp_Animation(PLAYER_MOVE_IDLE, true);
			m_eMoveState = MOVE_IDLE;
		}
	}

}

void CPlayer_Move::Dodge()
{
	if (m_pGameInstance->Key_Pressing('A'))
	{
		SetUp_Animation(PLAYER_MOVE_DODGE_L);
	}
	else if (m_pGameInstance->Key_Pressing('S'))
	{
		SetUp_Animation(PLAYER_MOVE_DODGE_B);
	}
	else if (m_pGameInstance->Key_Pressing('D'))
	{
		SetUp_Animation(PLAYER_MOVE_DODGE_R);
	}
	else
	{
		SetUp_Animation(PLAYER_MOVE_DODGE_F);
	}

	m_eMoveState = MOVE_DODGE;
	m_bMotionLock = true;
}

void CPlayer_Move::Dodge_Control()
{
	if (MOVE_DODGE == m_eMoveState && false == IsChanging())
	{
		if (true == m_isFinished)
		{
			SetUp_Animation(PLAYER_MOVE_IDLE, true);
			m_eMoveState = MOVE_IDLE;
		}
		if (15 < m_pModelCom->Get_KeyFrameIndex())
			m_bMotionLock = false;
	}
}

void CPlayer_Move::Move_In_Change(_float fTimeDelta)
{
	if (PLAYER_MOVE_WALK_F == m_AnimDesc.eNextPlayerAnim)
	{
		m_pTransformCom->Go_Straight(fTimeDelta * (*m_pSpeed * m_fChangeRate));
	}
	if (PLAYER_MOVE_WALK_L == m_AnimDesc.eNextPlayerAnim)
	{
		m_pTransformCom->Go_Left(fTimeDelta * (*m_pSpeed * m_fChangeRate));
	}
	if (PLAYER_MOVE_WALK_B == m_AnimDesc.eNextPlayerAnim)
	{
		m_pTransformCom->Go_Backward(fTimeDelta * (*m_pSpeed * m_fChangeRate));
	}
	if (PLAYER_MOVE_WALK_R == m_AnimDesc.eNextPlayerAnim)
	{
		m_pTransformCom->Go_Right(fTimeDelta * (*m_pSpeed * m_fChangeRate));
	}

	if (PLAYER_MOVE_JOG_F == m_AnimDesc.eNextPlayerAnim)
	{
		if (PLAYER_MOVE_WALK_F == m_AnimDesc.eCurrentPlayerAnim)
			m_fChangeRate = 1.f;

		m_pTransformCom->Go_Straight(fTimeDelta * (*m_pSpeed * 1.3f) * m_fChangeRate);
	}
	if (PLAYER_MOVE_JOG_L == m_AnimDesc.eNextPlayerAnim)
	{
		if (PLAYER_MOVE_WALK_L == m_AnimDesc.eCurrentPlayerAnim)
			m_fChangeRate = 1.f;

		m_pTransformCom->Go_Left(fTimeDelta * (*m_pSpeed * 1.3f) * m_fChangeRate);
	}
	if (PLAYER_MOVE_JOG_B == m_AnimDesc.eNextPlayerAnim)
	{
		if (PLAYER_MOVE_WALK_B == m_AnimDesc.eCurrentPlayerAnim)
			m_fChangeRate = 1.f;

		m_pTransformCom->Go_Backward(fTimeDelta * (*m_pSpeed * 1.3f) * m_fChangeRate);
	}
	if (PLAYER_MOVE_JOG_R == m_AnimDesc.eNextPlayerAnim)
	{
		if (PLAYER_MOVE_WALK_R == m_AnimDesc.eCurrentPlayerAnim)
			m_fChangeRate = 1.f;

		m_pTransformCom->Go_Right(fTimeDelta * (*m_pSpeed * 1.3f) * m_fChangeRate);
	}

}

void CPlayer_Move::Change_State()
{
	if (m_pGameInstance->Key_Down('Z'))
		m_pFSM->Set_State(CPlayer::STATE_ATTACK);
	else if (m_pGameInstance->Key_Pressing('X'))
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
