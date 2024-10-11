#include "stdafx.h"

#include "Boss_Lab.h"
#include "GameInstance.h"

#include "Boss_Lab_Body.h"
#include "Boss_Lab_Attack.h"

#include "Particle_Monster_Death.h"
#include "Boss_Lab_Teleport_Smoke.h"
#include "Boss_Lab_Teleport_Stone.h"

#include "Effect_Flare.h"

#include "Particle_LaunchStone.h"
#include "Particle_LaunchWaterDrop.h"

#include "UI_BossHPBar.h"
#include "UI_BossHPBarDeco.h"
#include "UI_BossHPBarGlow.h"

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
	m_iState = STATE_APPEAR;

	m_fMaxHP = 500.f;
	m_fHP = m_fMaxHP;
	m_fSpeed = 5.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	return S_OK;
}

_uint CBoss_Lab::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
		return OBJ_DEAD;

	for (auto& pPartObject : m_Parts)
	{
		if (nullptr == pPartObject)
			continue;
		pPartObject->Priority_Update(fTimeDelta);
	}

	if(true == Contain_State(STATE_APPEAR | STATE_CHARGE | STATE_TELEPORT | STATE_DEATH))
		m_pColliderCom->Set_OnCollision(false);
	else
		m_pColliderCom->Set_OnCollision(true);

	return OBJ_NOEVENT;
}

void CBoss_Lab::Update(_float fTimeDelta)
{
	_float fDistance = m_pTransformCom->Get_Distance(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

	if (m_fHP <= 0.f && false == Contain_State(STATE_DEATH))
	{
		m_iState = STATE_DEATH;
		m_bAnimStart = false;
		m_bAnimOver = false;
	}

	if (true == Contain_State(STATE_DEATH) && true == m_bAnimOver)
	{
		m_fRatio += fTimeDelta * 0.5f;
		static_cast<CParticle_Monster_Death*>(m_Parts[PART_EFFECT_DEATH])->Set_On();
	}

	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	vPlayerPos.m128_f32[1] = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];

	if(true == Contain_State(STATE_IDLE | STATE_WALK | STATE_APPEAR))
	{
		m_pTransformCom->LookAt(vPlayerPos, 0.1f);
	}
	else if (true == Contain_State(STATE_CHARGE))
	{
		m_pTransformCom->LookAt(vPlayerPos, 0.02f);
	}

	if (true == Contain_State(STATE_WALK))
	{
		m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed, m_pNavigationCom);
	}
	
	if (true == Contain_State(STATE_APPEAR))
	{
		CBoss_Lab_Body* pBody = dynamic_cast<CBoss_Lab_Body*>(m_Parts[PART_BODY]);

		if (fDistance < 30.f && false == m_bAggro)
		{
			pBody->Set_Intro(true);
			m_bAggro = true;
			Add_BossHPBar();
		}

		if (false == m_bEncounter && 31 <= pBody->Get_FrameIndex() && BOSS_LAB_TELEPORT_LAUNCH_ROAR == pBody->Get_CurrentAnim())
		{
			Add_LaunchEffect(Get_Position());
			static_cast<CBoss_Lab_Teleport_Smoke*>(m_Parts[PART_TELEPORT_SMOKE_SPLASH])->Set_On(Get_Position());
			m_bEncounter = true;
		}

		if(true == m_bAnimOver)
		{
			m_iState &= ~STATE_APPEAR;
			m_iState |= STATE_IDLE;
			m_fAttackDelay = 0.f;
		}
	}

	if(false == Contain_State(STATE_CHARGE | STATE_TELEPORT))
		m_bAttackActive_Body = false;
	if (false == Contain_State(STATE_NEAR))
	{
		m_bAttackActive_LH = false;
		m_bAttackActive_RH = false;
	}

	if (m_fHP / m_fMaxHP <= 0.5 && false== m_bFirstStun)
	{
		m_iState = STATE_STUN;
		m_bFirstStun = true;
		m_bAnimStart = false;
		m_bAnimOver = false;
	}

	if (true == Contain_State(STATE_IDLE | STATE_WALK) && false == Contain_State(STATE_APPEAR | STATE_STUN))
	{
		if (m_fAttackDelay > 2.f)
		{
			m_iState &= ~STATE_IDLE;
			m_iState &= ~STATE_WALK;
			if (fDistance < 15.f)
			{
				m_iState |= STATE_NEAR;
			}
			else if (fDistance < 25.f)
			{
				m_iState |= STATE_RUSH;
			}
			else
			{
				_uint iRandomNum = (_uint)m_pGameInstance->Get_Random(0.f, 2.f);

				//if (0 == iRandomNum)
				//	m_iState |= STATE_CHARGE;
				//else
				//{
					m_iState |= STATE_TELEPORT;
					static_cast<CBoss_Lab_Teleport_Smoke*>(m_Parts[PART_TELEPORT_SMOKE])->Set_On(Get_Position());
					static_cast<CBoss_Lab_Teleport_Stone*>(m_Parts[PART_TELEPORT_STONE])->Set_On(Get_Position());
					Add_TeleportEffect();
				//}
			}
		}
		else
		{
			if (fDistance < 10.f)
				m_iState = STATE_IDLE;
			else
				m_iState = STATE_WALK;
		}
	}
	else if (true == m_bAnimOver && false == Contain_State(STATE_WALK | STATE_DEATH))
	{
		m_iState = STATE_IDLE;
		m_fAttackDelay = 0.f;
	}

	m_fAttackDelay += fTimeDelta;

	if (true == Contain_State(STATE_TELEPORT))
	{
		CBoss_Lab_Body* pBody = static_cast<CBoss_Lab_Body*>(m_Parts[PART_BODY]);
		
		if (BOSS_LAB_TELEPORT_LAUNCH == pBody->Get_CurrentAnim() )
		{
			_vector vPos = Get_Position();
			vPos = XMVectorSetY(vPos, XMVectorGetY(m_pNavigationCom->Get_CellCenterPos(m_pNavigationCom->Get_CurrentCellIndex())));

			if(false == m_bLaunchStart)
			{
				static_cast<CBoss_Lab_Teleport_Smoke*>(m_Parts[PART_TELEPORT_SMOKE_BOIL])->Set_On(vPos);
				m_bLaunchStart = true;
			}
			if(false == m_bLaunchEffect && 35 <= pBody->Get_FrameIndex())
			{
				Add_LaunchEffect(vPos);
				static_cast<CBoss_Lab_Teleport_Smoke*>(m_Parts[PART_TELEPORT_SMOKE_SPLASH])->Set_On(vPos);
				m_bLaunchEffect = true;
			}
		}
		else
		{
			static_cast<CBoss_Lab_Teleport_Smoke*>(m_Parts[PART_TELEPORT_SMOKE_BOIL])->Set_Off();
			m_bLaunchEffect = false;
			m_bLaunchStart = false;
		}

		if(BOSS_LAB_TELEPORT_APPEAR == pBody->Get_CurrentAnim())
		{
			if (false == m_bLaunchOver && 2 <= pBody->Get_FrameIndex())
			{
				Add_LaunchEffect(Get_Position());
				static_cast<CBoss_Lab_Teleport_Smoke*>(m_Parts[PART_TELEPORT_SMOKE_SPLASH])->Set_On(Get_Position());
				m_bLaunchOver = true;
			}
		}
		else
		{
			m_bLaunchOver = false;
		}
	}

	for (auto& pPartObject : m_Parts)
	{
		if (nullptr == pPartObject)
			continue;
		pPartObject->Update(fTimeDelta);
	}

	if (false == Contain_State(STATE_TELEPORT | STATE_APPEAR))
	{
		m_pTransformCom->SetUp_OnCell(m_pNavigationCom);
	}
		

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CBoss_Lab::Late_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
	{
		if (nullptr == pPartObject)
			continue;
		pPartObject->Late_Update(fTimeDelta);
	}

	if (1.f < m_fRatio && true == static_cast<CParticle_Monster_Death*>(m_Parts[PART_EFFECT_DEATH])->Get_Dead())
		m_bDead = true;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CBoss_Lab::Render()
{
	return S_OK;
}

void CBoss_Lab::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
	__super::Intersect(strColliderTag, pCollisionObject, vSourInterval, vDestInterval);

}

void CBoss_Lab::Be_Damaged(_float fDamage, _fvector vAttackPos)
{
	m_fHP -= fDamage;

	BOSS_ANIM eAnim = static_cast<CBoss_Lab_Body*>(m_Parts[PART_BODY])->Get_CurrentAnim();

	if (true == Contain_State(STATE_STUN) && (BOSS_LAB_STUN_IDLE == eAnim || BOSS_LAB_STUN_IMPACT == eAnim))
		m_iState |= STATE_IMPACT;
}

HRESULT CBoss_Lab::Ready_Components()
{
	/* For.Com_Collider_AABB */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderAABBDesc{};
	ColliderAABBDesc.vExtents = _float3(4.f, 5.f, 4.f);
	ColliderAABBDesc.vCenter = _float3(0.f, ColliderAABBDesc.vExtents.y, 0.f);

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.pOwnerObject = this;
	ColliderDesc.pBoundingDesc = &ColliderAABBDesc;
	ColliderDesc.strColliderTag = TEXT("Coll_Monster");

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Lab::Ready_PartObjects()
{
	m_Parts.resize(PART_END);

	CBoss_Lab_Body::BODY_DESC BodyDesc = {};

	BodyDesc.fRotationPerSec = XMConvertToRadians(90.f);;
	BodyDesc.fSpeedPerSec = 1.f;
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	BodyDesc.pNavigationCom = m_pNavigationCom;
	BodyDesc.pBossTransformCom = m_pTransformCom;
	BodyDesc.pAnimStart = &m_bAnimStart;
	BodyDesc.pAnimOver = &m_bAnimOver;
	BodyDesc.pState = &m_iState;
	BodyDesc.pAttackActive_Body = &m_bAttackActive_Body;
	BodyDesc.pAttackActive_LH = &m_bAttackActive_LH;
	BodyDesc.pAttackActive_RH = &m_bAttackActive_RH;
	BodyDesc.pNoiseTextureCom = m_pNoiseTextureCom;
	BodyDesc.pRatio = &m_fRatio;

	if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Boss_Lab_Body"), &BodyDesc)))
		return E_FAIL;

	CBoss_Lab_Attack::ATTACK_DESC AttackDesc = {};

	AttackDesc.fRotationPerSec = 0.f;
	AttackDesc.fSpeedPerSec = 0.f;
	AttackDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	AttackDesc.pSocketMatrix = dynamic_cast<CBoss_Lab_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LB_Spine2");
	AttackDesc.vAngles = { 0.f, 0.f, 0.f };
	AttackDesc.vCenter = { 2.f, 2.f, 0.f };
	AttackDesc.vExtents = { 4.f, 4.f, 4.f };
	AttackDesc.pAttackActive = &m_bAttackActive_Body;
	AttackDesc.fDamage = 20;
	if (FAILED(__super::Add_PartObject(PART_ATTACK_BODY, TEXT("Prototype_GameObject_Boss_Lab_Attack"), &AttackDesc)))
		return E_FAIL;

	AttackDesc.pSocketMatrix = dynamic_cast<CBoss_Lab_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LB_Arm03_L");
	AttackDesc.vAngles = { 0.f, 0.f, XMConvertToRadians(-90.f) };
	AttackDesc.vCenter = { 3.f, 0.f, 0.f };
	AttackDesc.vExtents = { 1.f, 4.f, 1.f };
	AttackDesc.pAttackActive = &m_bAttackActive_LH;
	AttackDesc.fDamage = 10;
	if (FAILED(__super::Add_PartObject(PART_ATTACK_LH, TEXT("Prototype_GameObject_Boss_Lab_Attack"), &AttackDesc)))
		return E_FAIL;

	AttackDesc.pSocketMatrix = dynamic_cast<CBoss_Lab_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LB_Arm03_R");
	AttackDesc.pAttackActive = &m_bAttackActive_RH;
	AttackDesc.fDamage = 10;
	if (FAILED(__super::Add_PartObject(PART_ATTACK_RH, TEXT("Prototype_GameObject_Boss_Lab_Attack"), &AttackDesc)))
		return E_FAIL;

	CParticle_Monster_Death::PARTICLE_DEATH_DESC DeathDesc = {};
	DeathDesc.fRotationPerSec = 0.f;
	DeathDesc.fSpeedPerSec = 0.f;
	DeathDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	DeathDesc.pSocketMatrix = static_cast<CBoss_Lab_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LB_Spine1");;
	DeathDesc.iNumInstance = 5000;
	DeathDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);
	DeathDesc.vRange = _float3(10.f, 5.f, 10.f);
	DeathDesc.vExceptRange = _float3(0.f, 0.f, 0.f);
	DeathDesc.vSize = _float2(0.15f, 0.3f);
	DeathDesc.vSpeed = _float2(1.5f, 3.f);
	DeathDesc.vLifeTime = _float2(1.f, 2.f);;

	if (FAILED(__super::Add_PartObject(PART_EFFECT_DEATH, TEXT("Prototype_GameObject_Particle_Monster_Death"), &DeathDesc)))
		return E_FAIL;

	CBoss_Lab_Teleport_Smoke::BOSSTELEPORT_SMOKE_DESC SmokeDesc = {};
	SmokeDesc.fRotationPerSec = 0.f;
	SmokeDesc.fSpeedPerSec = 1.f;
	
	SmokeDesc.iNumInstance = 100;
	SmokeDesc.vCenter = { 0.f, 0.f, 0.f };
	SmokeDesc.vRange = { 15.f, 1.f, 15.f };
	SmokeDesc.vExceptRange = { 7.f, 1.f, 7.f };
	SmokeDesc.vSize = { 0.5f, 1.f };
	SmokeDesc.vSpeed = { 1.f, 3.f };
	SmokeDesc.vLifeTime = { 1.f, 2.f };
	SmokeDesc.vMinColor = { 0.f, 0.f, 0.f, 1.f };
	SmokeDesc.vMaxColor = { 1.f, 1.f, 1.f, 1.f };
	
	SmokeDesc.vScale = { 5.f, 5.f, 5.f };
	SmokeDesc.fSpeed = 3.f;
	SmokeDesc.fGravity = 2.f;
	SmokeDesc.vPivot = { 0.f, -8.f, 0.f };
	SmokeDesc.vColor = { 0.541f, 0.169f, 0.886f, 1.f };
	SmokeDesc.isLoop = false;
	if (FAILED(__super::Add_PartObject(PART_TELEPORT_SMOKE, TEXT("Prototype_GameObject_Boss_Lab_Teleport_Smoke"), &SmokeDesc)))
		return E_FAIL;

	SmokeDesc.fRotationPerSec = 0.f;
	SmokeDesc.fSpeedPerSec = 1.f;

	SmokeDesc.iNumInstance = 300;
	SmokeDesc.vCenter = { 0.f, 0.f, 0.f };
	SmokeDesc.vRange = { 15.f, 1.f, 15.f };
	SmokeDesc.vExceptRange = { 0.f, 0.f, 0.f };
	SmokeDesc.vSize = { 2.f, 4.f };
	SmokeDesc.vSpeed = { 4.f, 8.f };
	SmokeDesc.vLifeTime = { 0.5f, 1.f };
	SmokeDesc.vMinColor = { 0.f, 0.f, 0.f, 1.f };
	SmokeDesc.vMaxColor = { 1.f, 1.f, 1.f, 1.f };

	SmokeDesc.vScale = { 10.f, 10.f, 10.f };
	SmokeDesc.fSpeed = 1.f;
	SmokeDesc.fGravity = 5.f;
	SmokeDesc.vPivot = { 0.f, -5.f, 0.f };
	SmokeDesc.vColor = { 0.541f, 0.169f, 0.886f, 1.f };;
	SmokeDesc.isLoop = true;
	if (FAILED(__super::Add_PartObject(PART_TELEPORT_SMOKE_BOIL, TEXT("Prototype_GameObject_Boss_Lab_Teleport_Smoke"), &SmokeDesc)))
		return E_FAIL;

	SmokeDesc.fRotationPerSec = 0.f;
	SmokeDesc.fSpeedPerSec = 1.f;

	SmokeDesc.iNumInstance = 30;
	SmokeDesc.vCenter = { 0.f, 1.f, 0.f };
	SmokeDesc.vRange = { 8.f, 4.f, 8.f };
	SmokeDesc.vExceptRange = { 0.f, 0.f, 0.f };
	SmokeDesc.vSize = { 2.f, 4.f };
	SmokeDesc.vSpeed = { 10.f, 15.f };
	SmokeDesc.vLifeTime = { 1.f, 2.f };
	SmokeDesc.vMinColor = { 0.f, 0.f, 0.f, 1.f };
	SmokeDesc.vMaxColor = { 1.f, 1.f, 1.f, 1.f };

	SmokeDesc.vScale = { 1.f, 1.f, 1.f };
	SmokeDesc.fSpeed = 1.f;
	SmokeDesc.fGravity = 7.f;
	SmokeDesc.vPivot = { 0.f, -3.f, 0.f };
	SmokeDesc.vColor = { 0.541f, 0.169f, 0.886f, 1.f };;
	SmokeDesc.isLoop = false;
	if (FAILED(__super::Add_PartObject(PART_TELEPORT_SMOKE_SPLASH, TEXT("Prototype_GameObject_Boss_Lab_Teleport_Smoke"), &SmokeDesc)))
		return E_FAIL;

	CBoss_Lab_Teleport_Stone::BOSSTELEPORT_STONE_DESC StoneDesc = {};

	StoneDesc.fRotationPerSec = 0.f;
	StoneDesc.fSpeedPerSec = 1.f;
	StoneDesc.fSpeed = 1.f;
	StoneDesc.fGravity = 3.f;
	if (FAILED(__super::Add_PartObject(PART_TELEPORT_STONE, TEXT("Prototype_GameObject_Boss_Lab_Teleport_Stone"), &StoneDesc)))
		return E_FAIL;

	return S_OK;
}

_bool CBoss_Lab::Contain_State(_uint iState)
{
	if (0 != (m_iState & iState))
		return true;

	return false;
}

HRESULT CBoss_Lab::Add_TeleportEffect()
{
	CEffect_Flare::FLARE_DESC LightDesc = {};

	LightDesc.fRotationPerSec = 0.f;
	LightDesc.fSpeedPerSec = 0.f;
	LightDesc.vColor = { 0.541f, 0.169f, 0.886f, 0.f };
	LightDesc.vScale = { 100.f, 100.f, 100.f };
	XMStoreFloat3(&LightDesc.vPos, Get_Position());

	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Flare"), &LightDesc);

	return S_OK;
}

HRESULT CBoss_Lab::Add_LaunchEffect(_fvector vPos)
{
	CParticle_LaunchStone::PARTICLE_LAUNCHSTONE_DESC StoneDesc{};
	StoneDesc.fRotationPerSec = 0.f;
	StoneDesc.fSpeedPerSec = 0.f;
	StoneDesc.vPivot = {0.f, -10.f, 0.f};
	XMStoreFloat3(&StoneDesc.vPos, vPos);

	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_LaunchStone"), &StoneDesc);

	CParticle_LaunchWaterDrop::PARTICLE_LAUNCHWATERDROP_DESC WaterDropDesc{};
	WaterDropDesc.fRotationPerSec = 0.f;
	WaterDropDesc.fSpeedPerSec = 0.f;
	WaterDropDesc.vPivot = { 0.f, -10.f, 0.f };
	WaterDropDesc.vPos = StoneDesc.vPos;

	m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_LaunchWaterDrop"), &WaterDropDesc);

	return S_OK;
}

HRESULT CBoss_Lab::Add_BossHPBar()
{
	CUI_BossHPBar::UI_HPBAR_DESC BarDesc = {};

	BarDesc.fRotationPerSec = 0.f;
	BarDesc.fSpeedPerSec = 1.f;
	BarDesc.fSizeX = 930.f;
	BarDesc.fSizeY = 14.25f;
	BarDesc.fX = g_iWinSizeX * 0.5f;
	BarDesc.fY = 650.f;
	BarDesc.pMonster = this;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_BossHPBar"), &BarDesc)))
		return E_FAIL;

	CUI_BossHPBarDeco::UI_HPBARDECO_DESC DecoDesc = {};

	DecoDesc.fRotationPerSec = 0.f;
	DecoDesc.fSpeedPerSec = 1.f;
	DecoDesc.fSizeX = 372.f;
	DecoDesc.fSizeY = 44.f;
	DecoDesc.fX = g_iWinSizeX * 0.5f;
	DecoDesc.fY = 680.f;
	DecoDesc.pMonster = this;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_BossHPBar_Deco"), &DecoDesc)))
		return E_FAIL;

	CUI_BossHPBarGlow::UI_HPBARGLOW_DESC GlowDesc = {};

	GlowDesc.fRotationPerSec = 0.f;
	GlowDesc.fSpeedPerSec = 1.f;
	GlowDesc.fSizeX = 744.f;
	GlowDesc.fSizeY = 88.f;
	GlowDesc.fX = g_iWinSizeX * 0.5f;
	GlowDesc.fY = 598.f;
	GlowDesc.pMonster = this;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_BossHPBar_Glow"), &GlowDesc)))
		return E_FAIL;

	return S_OK;
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

	Safe_Release(m_pColliderCom);
}
