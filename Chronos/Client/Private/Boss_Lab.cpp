#include "stdafx.h"

#include "Boss_Lab.h"

#include "Boss_Lab_Body.h"

#include "GameInstance.h"

CBoss_Lab::CBoss_Lab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CBoss_Lab::CBoss_Lab(const CBoss_Lab& Prototype)
	: CMonster(Prototype)
{
}

HRESULT CBoss_Lab::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Lab::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_eBossAnim = BOSS_LAB_IDLE;

	m_pPlayerTransformCom = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), g_strTransformTag, 0));
	Safe_AddRef(m_pPlayerTransformCom);

	m_pTransformCom->LookAt(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

	return S_OK;
}

void CBoss_Lab::Priority_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
	{
		if (nullptr == pPartObject)
			continue;
		pPartObject->Priority_Update(fTimeDelta);
	}

}

void CBoss_Lab::Update(_float fTimeDelta)
{
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDir = vPlayerPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_float fDistance = XMVectorGetX(XMVector3Length(vDir));

	vDir = XMVector3Normalize(vDir);
	vLook = XMVector3Normalize(vLook);

	_float fDot = XMVectorGetX(XMVector3Dot(vDir, vLook));
	_float fRadian = 0.f;

	if (fDot > 1.f)
		fRadian = 0.f;
	else if (fDot < -1.f)
		fRadian = XMConvertToRadians(180.f);
	else
		fRadian = acosf(fDot);

	if (XMVectorGetY(XMVector3Cross(vLook, vDir)) < 0.f)
		fRadian = fRadian * -1.f;

	if (BOSS_LAB_ATK_CHARGE == m_eBossAnim)
	{
		m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed * 5.f, m_pNavigationCom);
		m_fChargeTime += fTimeDelta;
	}

	if (BOSS_LAB_ATK_BEAM_INTO == m_eBossAnim)
		TurnTo_Player(fRadian, fTimeDelta);

	if(BOSS_LAB_ATK_CHARGE_INTO == m_eBossAnim)
		TurnTo_Player(fRadian, fTimeDelta);

	if (false == m_bMotionLock)
	{
		if (m_fAttackDelay < 3.f)
		{
			TurnTo_Player(fRadian, fTimeDelta);

			if (fDistance > 5.f)
			{
				m_eBossAnim = BOSS_LAB_WALKF;
				m_eState = STATE_MOVE;
				m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed, m_pNavigationCom);
			}
			else
			{
				m_eBossAnim = BOSS_LAB_IDLE;
				m_eState = STATE_IDLE;
			}
		}
		else
		{
			Attack_Type_Desc(fDistance);
		}
#pragma region 테스트용
		//if(false == m_Test)
		//{
		//	m_eState = STATE_TELEPORT;
		//	m_eBossAnim = BOSS_LAB_TELEPORT_START;
		//	m_bMotionLock = true;
		//	m_Test = true;
		//}
#pragma endregion
	}
	m_fAttackDelay += fTimeDelta;

	if (STATE_TELEPORT == m_eState)
		Teleport_Progress(fTimeDelta);

	if (true == m_isFinished)
	{
		if (STATE_TELEPORT == m_eState)
			Teleport_Control();
		else
			Finished_Animation_Control();
	}


	for (auto& pPartObject : m_Parts)
	{
		if (nullptr == pPartObject)
			continue;
		pPartObject->Update(fTimeDelta);
	}

	__super::Update(fTimeDelta);
}

void CBoss_Lab::Late_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
	{
		if (nullptr == pPartObject)
			continue;
		pPartObject->Late_Update(fTimeDelta);
	}

}

HRESULT CBoss_Lab::Render()
{
	return S_OK;
}

HRESULT CBoss_Lab::Ready_Components()
{

	return S_OK;
}

HRESULT CBoss_Lab::Ready_PartObjects()
{
	m_Parts.resize(PART_END);

	CBoss_Lab_Body::BODY_DESC BodyDesc = {};

	BodyDesc.fRotationPerSec = 90.f;
	BodyDesc.fSpeedPerSec = 1.f;
	BodyDesc.pBossAnim = &m_eBossAnim;
	BodyDesc.pIsFinished = &m_isFinished;
	BodyDesc.pNavigationCom = m_pNavigationCom;
	BodyDesc.pBossTransformCom = m_pTransformCom;
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

	if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Boss_Lab_Body"), &BodyDesc)))
		return E_FAIL;

	return S_OK;
}

void CBoss_Lab::Attack_Type_Desc(_float fDistance)
{
	// 근접 공격
	if (m_fMeleeAttack_Dist > fDistance)
	{
		m_eState = STATE_ATTACK;

		_uint m_iNum = (_uint)m_pGameInstance->Get_Random(0.f, 2.f);

		if (0 == m_iNum)
			m_eBossAnim = BOSS_LAB_ATK_SWIPE_L;
		else
			m_eBossAnim = BOSS_LAB_ATK_SWIPE_R;

		m_bMotionLock = true;
	}
	// 더블 스윕
	else if (m_fNearAttack_Dist > fDistance)
	{
		m_eState = STATE_ATTACK;
		m_eBossAnim = BOSS_LAB_ATK_DOUBLE_SWIPE;

		m_bMotionLock = true;
	}
	// 러쉬
	else if (m_fMidRangeAttack_Dist > fDistance)
	{
		m_eState = STATE_ATTACK;
		m_eBossAnim = BOSS_LAB_ATK_RUSH;

		m_bMotionLock = true;
	}
	// 빔 아니면 구르기
	else if (m_fLongRangeAttack_Dist > fDistance)
	{
		m_eState = STATE_ATTACK;
		_uint m_iNum = (_uint)m_pGameInstance->Get_Random(1.f, 3.f);

		if (1 == m_iNum)
			m_eBossAnim = BOSS_LAB_ATK_BEAM_INTO;
		else
			m_eBossAnim = BOSS_LAB_ATK_CHARGE;

		m_bMotionLock = true;
	}
	else
	{
		// 원래 자리로 돌아가기.
		m_eBossAnim = BOSS_LAB_IDLE;
		m_eState = STATE_IDLE;
	}

}

void CBoss_Lab::Finished_Animation_Control()
{
	switch (m_eBossAnim)
	{
	case BOSS_LAB_ATK_CHARGE_INTO:
		m_eBossAnim = BOSS_LAB_ATK_CHARGE;
		break;
	case BOSS_LAB_ATK_CHARGE:
		if(m_fChargeTime > 1.5f)
		{
			m_eBossAnim = BOSS_LAB_ATK_CHARGE_OUT;
			m_fChargeTime = 0.f;
		}
		break;
	case BOSS_LAB_ATK_BEAM_INTO:
		m_eBossAnim = BOSS_LAB_ATK_BEAM;
		break;
	case BOSS_LAB_ATK_BEAM:
		m_eBossAnim = BOSS_LAB_ATK_BEAM_OUT;
		break;

	default:
		m_bMotionLock = false;
		m_fAttackDelay = 0.f;
		m_eBossAnim = BOSS_LAB_IDLE;
		m_eState = STATE_IDLE;
		break;
	}
}

void CBoss_Lab::Teleport_Control()
{
	if (BOSS_LAB_TELEPORT_START == m_eBossAnim)
	{
		m_eBossAnim = BOSS_LAB_TELEPORT_LAUNCH_UNDERGROUND;
	}
	else if (BOSS_LAB_TELEPORT_LAUNCH == m_eBossAnim)
	{
		++m_iTeleportAttack_Count;
		m_eBossAnim = BOSS_LAB_TELEPORT_LAUNCH_UNDERGROUND;
		m_fTeleportDelay = 0.f;
	}
	else if(BOSS_LAB_TELEPORT_APPEAR == m_eBossAnim)
	{
		m_bMotionLock = false;
		m_iTeleportAttack_Count = 0;
		m_fAttackDelay = 0.f;
		m_fTeleportDelay = 0.f;
		m_eBossAnim = BOSS_LAB_IDLE;
		m_eState = STATE_IDLE;
	}

}

void CBoss_Lab::Teleport_Progress(_float fTimeDelta)
{
	if (BOSS_LAB_TELEPORT_LAUNCH_UNDERGROUND == m_eBossAnim && 3.f < m_fTeleportDelay)
	{
		if (m_iTeleportAttack_Count >= 3)
		{
			m_eBossAnim = BOSS_LAB_TELEPORT_APPEAR;
		}
		else
		{
			_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
			vPlayerPos.m128_f32[1] = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);

			m_eBossAnim = BOSS_LAB_TELEPORT_LAUNCH;
		}

		m_fTeleportDelay = 0.f;
	}

	m_fTeleportDelay += fTimeDelta;
}

void CBoss_Lab::TurnTo_Player(_float fRadian, _float fTimeDelta)
{
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);

	if (abs(XMConvertToDegrees(fRadian)) < 10.f)
		m_pTransformCom->LookAt(vPlayerPos);
	else
	{
		if (fRadian > 0.f)
			m_pTransformCom->Turn(false, true, false, fTimeDelta);
		else
			m_pTransformCom->Turn(false, true, false, -fTimeDelta);
	}

}

CBoss_Lab* CBoss_Lab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Lab* pInstance = new CBoss_Lab(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CBoss_Lab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Lab::Clone(void* pArg)
{

	CBoss_Lab* pInstance = new CBoss_Lab(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CBoss_Lab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Lab::Free()
{
	__super::Free();

	Safe_Release(m_pPlayerTransformCom);
}
