#include "stdafx.h"
#include "Player_Action.h"
#include "GameInstance.h"

#include "Player.h"
#include "Player_Item.h"

#include "Particle_Teleport.h"
#include "Trail_Revolve.h"

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
			Add_Particle_Teleport();
			Add_TrailRevolve();
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
					Add_Particle_Teleport();
					Add_TrailRevolve();

					SOUND_DESC desc = {};
					desc.fVolume = 1.f;

					m_pGameInstance->SoundPlay_Additional(TEXT("SFX_Lab_Teleporter_PrePort_02.ogg"), desc);
					m_pGameInstance->SoundPlay_Additional(TEXT("lab_teleporter_flutter_1.ogg"), desc);
					m_pGameInstance->SoundPlay_Additional(TEXT("lab_teleporter_whoosh_high_5.ogg"), desc);
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

HRESULT CPlayer_Action::Add_Particle_Teleport()
{
	CParticle_Teleport::PARTICLE_TELEPORT_DESC desc = {};

	desc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.fSpeedPerSec = 1.f;
	XMStoreFloat3(&desc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Teleport"), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Action::Add_TrailRevolve()
{
	CTrail_Revolve::TRAIL_REVOLOVE_DESC desc = {};

	desc.iTrailCount = 20;
	desc.vColor = _float4(0.0f, 0.749f, 1.0f, 1.f);
	XMStoreFloat3(&desc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	desc.vRange = _float3(4.f, 4.f, 4.f);
	desc.fAccel = 5.f;
	desc.fSpeed = 10.f;
	desc.fScale = 0.3f;
	desc.fRotaionPerSecond = XMConvertToRadians(360.f);
	desc.eType = CTrail_Revolve::TYPE_CONSTANT;
	desc.fTypeAccel = 0.03f;
	desc.fTime = 1.5f;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Trail"), TEXT("Prototype_GameObject_Trail_Revolve"), &desc)))
		return E_FAIL;



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
