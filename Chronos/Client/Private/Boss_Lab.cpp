#include "stdafx.h"

#include "Boss_Lab.h"

#include "Boss_Lab_Body.h"
#include "Boss_Lab_Attack.h"

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

	m_iState = STATE_APPEAR;

	m_iMaxHP = 100;
	m_iHP = m_iMaxHP;
	m_fSpeed = 5.f;


	return S_OK;
}

_uint CBoss_Lab::Priority_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
	{
		if (nullptr == pPartObject)
			continue;
		pPartObject->Priority_Update(fTimeDelta);
	}

	if(true == Contain_State(STATE_APPEAR) || true == Contain_State(STATE_CHARGE) || true == Contain_State(STATE_TELEPORT))
		m_pColliderCom->Set_OnCollision(false);
	else
		m_pColliderCom->Set_OnCollision(true);

	return OBJ_NOEVENT;
}

void CBoss_Lab::Update(_float fTimeDelta)
{
	_float fDistance = m_pTransformCom->Get_Distance(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

	if (0 >= m_iHP)
	{
		m_iState = STATE_DEATH;
		m_bAnimOver = false;
	}

	if(Contain_State(STATE_IDLE | STATE_WALK | STATE_APPEAR))
	{
		_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
		vPlayerPos.m128_f32[1] = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];
		m_pTransformCom->LookAt(vPlayerPos, 0.01f);
	}

	if (true == Contain_State(STATE_WALK))
	{
		m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed, m_pNavigationCom);
	}

	if (true == Contain_State(STATE_APPEAR))
	{
		if (fDistance < 30.f)
		{
			dynamic_cast<CBoss_Lab_Body*>(m_Parts[PART_BODY])->Set_Intro(true);
		}
		if(true == m_bAnimOver)
		{
			m_iState &= ~STATE_APPEAR;
			m_iState |= STATE_IDLE;
			m_fAttackDelay = 0.f;
		}
	}

	if (m_iHP / (_float)m_iMaxHP <= 0.5 && false== m_bFirstStun)
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

				if (0 == iRandomNum)
					m_iState |= STATE_CHARGE;
				else
					m_iState |= STATE_TELEPORT;
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
	else if (true == m_bAnimOver && false == Contain_State(STATE_WALK))
	{
		m_iState = STATE_IDLE;
		m_fAttackDelay = 0.f;
	}

	m_fAttackDelay += fTimeDelta;

	for (auto& pPartObject : m_Parts)
	{
		if (nullptr == pPartObject)
			continue;
		pPartObject->Update(fTimeDelta);
	}

	__super::Update(fTimeDelta);

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
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

}

HRESULT CBoss_Lab::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif
	return S_OK;
}

void CBoss_Lab::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
	__super::Intersect(strColliderTag, pCollisionObject, vSourInterval, vDestInterval);

}

void CBoss_Lab::Be_Damaged(_uint iDamage, _fvector vAttackPos)
{
	m_iHP -= iDamage;

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
	
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pGameInstance->Add_Collider_OnLayers(TEXT("Coll_Monster"), m_pColliderCom);

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
	AttackDesc.iDamage = 20;
	if (FAILED(__super::Add_PartObject(PART_ATTACK_BODY, TEXT("Prototype_GameObject_Boss_Lab_Attack"), &AttackDesc)))
		return E_FAIL;

	AttackDesc.pSocketMatrix = dynamic_cast<CBoss_Lab_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LB_Arm03_L");
	AttackDesc.vAngles = { 0.f, 0.f, XMConvertToRadians(-90.f) };
	AttackDesc.vCenter = { 3.f, 0.f, 0.f };
	AttackDesc.vExtents = { 1.f, 4.f, 1.f };
	AttackDesc.pAttackActive = &m_bAttackActive_LH;
	AttackDesc.iDamage = 10;
	if (FAILED(__super::Add_PartObject(PART_ATTACK_LH, TEXT("Prototype_GameObject_Boss_Lab_Attack"), &AttackDesc)))
		return E_FAIL;

	AttackDesc.pSocketMatrix = dynamic_cast<CBoss_Lab_Body*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("Bone_LB_Arm03_R");
	AttackDesc.pAttackActive = &m_bAttackActive_RH;
	AttackDesc.iDamage = 10;
	if (FAILED(__super::Add_PartObject(PART_ATTACK_RH, TEXT("Prototype_GameObject_Boss_Lab_Attack"), &AttackDesc)))
		return E_FAIL;

	return S_OK;
}

_bool CBoss_Lab::Contain_State(_uint iState)
{
	if (0 != (m_iState & iState))
		return true;

	return false;
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
