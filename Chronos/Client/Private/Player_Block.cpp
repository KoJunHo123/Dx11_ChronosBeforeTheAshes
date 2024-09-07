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

	PLAYER_STATE_BLOCK_DESC* pDesc = static_cast<PLAYER_STATE_BLOCK_DESC*>(pArg);

	m_pStamina = pDesc->pStamina;

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

			
			if(true == m_bImpact)
			{
				if (*m_pStamina < 0)
					*m_pPlayerAnim = PLAYER_BLOCK_IMPACT_BREAK;
				else
					*m_pPlayerAnim = PLAYER_BLOCK_IMPACT;

				m_bMotionLock = true;
			}

			if (m_pGameInstance->Key_Down(VK_LBUTTON))
				m_pFSM->Set_State(CPlayer::STATE_ATTACK);
		}
		else
		{
			m_pFSM->Set_State(CPlayer::STATE_MOVE);
		}
	}

	if (true == *m_pIsFinished)
	{
		if (PLAYER_BLOCK_IMPACT == *m_pPlayerAnim
			|| PLAYER_BLOCK_IMPACT_BREAK == *m_pPlayerAnim)
		{
			m_bMotionLock = false;
			m_bImpact = false;
		}
	}


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
	_bool bKeyPress = { false };
	if (m_pGameInstance->Key_Pressing('W'))
	{
		*m_pPlayerAnim = PLAYER_BLOCK_WALK_F;
		m_eBlockState = BLOCK_WALK;

		m_pTransformCom->LookDir(XMLoadFloat3(m_pCameraLook), 0.1f);
		m_pTransformCom->Go_Straight(fTimeDelta * (*m_pSpeed * 0.7f));

		bKeyPress = true;
	}
	if (m_pGameInstance->Key_Pressing('S'))
	{
		*m_pPlayerAnim = PLAYER_BLOCK_WALK_B;
		m_eBlockState = BLOCK_WALK;

		m_pTransformCom->LookDir(XMLoadFloat3(m_pCameraLook), 0.1f);
		m_pTransformCom->Go_Backward(fTimeDelta * (*m_pSpeed * 0.7f));

		bKeyPress = true;
	}
	if (m_pGameInstance->Key_Pressing('A'))
	{
		*m_pPlayerAnim = PLAYER_BLOCK_WALK_L;
		m_eBlockState = BLOCK_WALK;

		m_pTransformCom->LookDir(XMLoadFloat3(m_pCameraLook), 0.1f);
		m_pTransformCom->Go_Left(fTimeDelta * (*m_pSpeed * 0.7f));

		bKeyPress = true;
	}
	if (m_pGameInstance->Key_Pressing('D'))
	{
		*m_pPlayerAnim = PLAYER_BLOCK_WALK_R;
		m_eBlockState = BLOCK_WALK;

		m_pTransformCom->LookDir(XMLoadFloat3(m_pCameraLook), 0.1f);
		m_pTransformCom->Go_Right(fTimeDelta * (*m_pSpeed * 0.7f));

		bKeyPress = true;
	}
	if(false == bKeyPress)
	{
		*m_pPlayerAnim = PLAYER_BLOCK_IDLE;
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
