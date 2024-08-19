#include "stdafx.h"
#include "Player_Block.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer_Block::CPlayer_Block()
{
}

HRESULT CPlayer_Block::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

HRESULT CPlayer_Block::StartState(void** pArg)
{
	__super::StartState(pArg);

	return S_OK;
}

void CPlayer_Block::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPlayer_Block::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


	if (false == m_bMotionLock)
	{
		if (m_pGameInstance->Key_Pressing(VK_RBUTTON))
		{
			Walk(fTimeDelta);

			if (m_pGameInstance->Key_Down(VK_LBUTTON))
				m_pFSM->Set_State(CPlayer::STATE_ATTACK);
		}
		else
		{
			m_pFSM->Set_State(CPlayer::STATE_MOVE);
		}
	}

	Play_Animation(fTimeDelta);

}

void CPlayer_Block::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

}

HRESULT CPlayer_Block::Render()
{
	__super::Render();
	return S_OK;
}

HRESULT CPlayer_Block::ExitState(void** pArg)
{
	__super::ExitState(pArg);
	m_eBlockState = BLOCK_END;

	return S_OK;
}

void CPlayer_Block::Walk(_float fTimeDelta)
{

	if (m_pGameInstance->Key_Pressing('W'))
	{
		m_pModelCom->SetUp_Animation(PLAYER_BLOCK_WALK_F);
		m_eBlockState = BLOCK_WALK;

		m_pTransformCom->Go_Straight(fTimeDelta * (*m_pSpeed * 0.7f));

	}
	else if (m_pGameInstance->Key_Pressing('A'))
	{
		m_pModelCom->SetUp_Animation(PLAYER_BLOCK_WALK_L);
		m_eBlockState = BLOCK_WALK;

		m_pTransformCom->Go_Left(fTimeDelta * (*m_pSpeed * 0.7f));

	}
	else if (m_pGameInstance->Key_Pressing('S'))
	{
		m_pModelCom->SetUp_Animation(PLAYER_BLOCK_WALK_B);
		m_eBlockState = BLOCK_WALK;

		m_pTransformCom->Go_Backward(fTimeDelta * (*m_pSpeed * 0.7f));

	}
	else if (m_pGameInstance->Key_Pressing('D'))
	{
		m_pModelCom->SetUp_Animation(PLAYER_BLOCK_WALK_R);
		m_eBlockState = BLOCK_WALK;

		m_pTransformCom->Go_Right(fTimeDelta * (*m_pSpeed * 0.7f));

	}
	else
	{
		m_pModelCom->SetUp_Animation(PLAYER_BLOCK_IDLE, true);
		m_eBlockState = BLOCK_IDLE;
	}


}

CPlayer_Block* CPlayer_Block::Create(void* pArg)
{
	CPlayer_Block* pInstance = new CPlayer_Block();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_Block"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Block::Free()
{
	__super::Free();
}
