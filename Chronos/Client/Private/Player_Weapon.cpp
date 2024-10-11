#include "stdafx.h"
#include "Player_Weapon.h"
#include "GameInstance.h"
#include "Player.h"

#include "Monster.h"

#include "Particle_AttackLight.h"


CPlayer_Weapon::CPlayer_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Part(pDevice, pContext)
{
}

CPlayer_Weapon::CPlayer_Weapon(const CPlayer_Weapon& Prototype)
	: CPlayer_Part (Prototype)
{
}

HRESULT CPlayer_Weapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Weapon::Initialize(void* pArg)
{
	PLAYER_WEAPON_DESC* pDesc = static_cast<PLAYER_WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
	m_pTailSocketMatrix = pDesc->pTailSocketMatrix;

	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pNoiseTextureCom = pDesc->pNoiseTextureCom;
	Safe_AddRef(m_pNoiseTextureCom);

	m_pRatio = pDesc->pRatio;
	m_pDrain = pDesc->pDrain;
	m_pHP = pDesc->pHP;
	m_fMaxHP = pDesc->fMaxHP;
	m_pStamina = pDesc->pStamina;
	m_pSkillGage = pDesc->pSkillGage;
	m_fMaxSkillGage = pDesc->fMaxSkillGage;
	m_pSkillDuration = pDesc->pSkillDuration;

	m_fDamage = 10.f;

	return S_OK;
}

_uint CPlayer_Weapon::Priority_Update(_float fTimeDelta)
{
	if(CPlayer::STATE_ATTACK == m_pFSM->Get_State())
	{
		if(true == IsAttackAnim() && 5 < *m_pFrameIndex && 15 > *m_pFrameIndex)
		{
			m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), 90.f);
			if(*m_pSkillDuration < 0.f)
				m_pColliderCom->Set_OnCollision(true);
			else
				m_pSkillColliderCom->Set_OnCollision(true);
			if (false == m_bStaminaDown)
			{
				*m_pStamina -= 10.f;
				if (*m_pStamina < 0.f)
					*m_pStamina = 0.f;

				m_bStaminaDown = true;
			}
		}
		else
		{
			m_bStaminaDown = false;
			m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), 0.f);
			m_pColliderCom->Set_OnCollision(false);
			m_pSkillColliderCom->Set_OnCollision(false);
		}
	}
	XMStoreFloat3(&m_vPrePosition, XMLoadFloat4x4(&m_WorldMatrix).r[3]);

	return OBJ_NOEVENT;
}

void CPlayer_Weapon::Update(_float fTimeDelta)
{
	_matrix TailSocketMatrix = XMLoadFloat4x4(m_pTailSocketMatrix);
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	_float3 vScale = _float3(XMVectorGetX(XMVector3Length(SocketMatrix.r[0])),
		XMVectorGetX(XMVector3Length(SocketMatrix.r[1])),
		XMVectorGetX(XMVector3Length(SocketMatrix.r[2])));

	_vector vPosition = SocketMatrix.r[3];

	_vector		vLook = TailSocketMatrix.r[3] - vPosition;
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	XMStoreFloat3((_float3*)&(*m_pSocketMatrix).m[0][0], XMVector3Normalize(vRight) * vScale.x);
	XMStoreFloat3((_float3*)&(*m_pSocketMatrix).m[1][0], XMVector3Normalize(vUp) * vScale.y);
	XMStoreFloat3((_float3*)&(*m_pSocketMatrix).m[2][0], XMVector3Normalize(vLook) * vScale.z);

	SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	// ??
	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	// 내 위치 * 붙여야 할 뼈의 위치 * 플레이어 위치 -> 플레이어의 위치에서 붙여야 할 뼈의 위치.
	// -> 셰이더에서 해주던 뼈 * 월드를 여기서 하는 거.
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	_matrix TailWorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * TailSocketMatrix * XMLoadFloat4x4(m_pParentMatrix);
	XMStoreFloat3(&m_vTailPos, TailWorldMatrix.r[3]);
	
	m_pColliderCom->Update(&m_WorldMatrix);
	m_pSkillColliderCom->Update(&m_WorldMatrix);

	// 만간에 지우자.
	if (m_pGameInstance->Get_DIKeyState_Down(DIKEYBOARD_P))
	{
		if (0.f == m_fDamage)
			m_fDamage = 10.f;
		else if (10.f == m_fDamage)
			m_fDamage = 100.f;
		else
			m_fDamage = 0.f;
	}
}

void CPlayer_Weapon::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

#ifdef _DEBUG
	if (*m_pSkillDuration < 0.f)
		m_pGameInstance->Add_DebugObject(m_pColliderCom);
	else
		m_pGameInstance->Add_DebugObject(m_pSkillColliderCom);
#endif
}

HRESULT CPlayer_Weapon::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", m_pRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pNoiseTextureCom->Bind_ShadeResource(m_pShaderCom, "g_NoiseTexture", 3)))
		return E_FAIL;

	_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vEmissiveColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ComboTexture", aiTextureType_COMBO, i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer_Weapon::Render_LightDepth()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_SHADOWVIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}


void CPlayer_Weapon::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
	_float fDamage = m_fDamage;
	if (PLAYER_ATK_POWER_01 == *m_pPlayerAnim || PLAYER_ATK_POWER_02 == *m_pPlayerAnim || PLAYER_ATK_RUN == *m_pPlayerAnim)
		fDamage = m_fDamage * 2;

	if (TEXT("Coll_Monster") == strColliderTag)
	{
		*m_pSkillGage += fDamage;

		if (m_fMaxSkillGage < *m_pSkillGage)
			*m_pSkillGage = m_fMaxSkillGage;

		if (true == *m_pDrain)
		{
			_float fHP = *m_pHP + fDamage;
			if (m_fMaxHP < fHP)
				*m_pHP = m_fMaxHP;
			else
				*m_pHP = fHP;
		}

		CMonster* pMonster = static_cast<CMonster*>(pCollisionObject);
		pMonster->Be_Damaged(fDamage, XMLoadFloat4x4(m_pParentMatrix).r[3]);

		_vector vTargetPos = pCollisionObject->Get_Position();
		vTargetPos.m128_f32[1] = m_vTailPos.y;
		_vector vDir = vTargetPos - XMLoadFloat3(&m_vTailPos);
		_vector vPos = XMLoadFloat3(&m_vTailPos) + vDir * 0.5f;
		_vector vMoveDir = XMLoadFloat4x4(&m_WorldMatrix).r[3] - XMLoadFloat3(&m_vPrePosition);

		Add_AttackParticle(vPos, XMVector3Normalize(vMoveDir) * 0.1f);
	}
}

HRESULT CPlayer_Weapon::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Sword"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderOBBDesc{};
	ColliderOBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	ColliderOBBDesc.vExtents = _float3(.25f, .25f, 1.2f);
	ColliderOBBDesc.vCenter = _float3(0.f, 0.f, 0.8f);

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.pOwnerObject = this;
	ColliderDesc.pBoundingDesc = &ColliderOBBDesc;
	ColliderDesc.bCollisionOnce = true;
	ColliderDesc.strColliderTag = TEXT("Coll_Player_Attack");

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC			ColliderSkillOBBDesc{};
	ColliderSkillOBBDesc.vAngles = _float3(0.f, 0.f, 0.f);
	ColliderSkillOBBDesc.vExtents = _float3(.5f, .5f, 2.4f);
	ColliderSkillOBBDesc.vCenter = _float3(0.f, 0.f, 1.6f);

	CCollider::COLLIDER_DESC ColliderSkillDesc = {};
	ColliderSkillDesc.pOwnerObject = this;
	ColliderSkillDesc.pBoundingDesc = &ColliderSkillOBBDesc;
	ColliderSkillDesc.bCollisionOnce = true;
	ColliderSkillDesc.strColliderTag = TEXT("Coll_Player_Attack");

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_Skill"), reinterpret_cast<CComponent**>(&m_pSkillColliderCom), &ColliderSkillDesc)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CPlayer_Weapon::Add_AttackParticle(_fvector vPos, _fvector vPivot)
{
	CParticle_AttackLight::PARTICLE_SPARK_DESC desc = {};

	desc.fRotationPerSec = 0.f;
	desc.fSpeedPerSec = 1.f;
	XMStoreFloat3(&desc.vPos, vPos);
	XMStoreFloat3(&desc.vPivot, vPivot);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Spark"), &desc)))
		return E_FAIL;

	return S_OK;
}

_bool CPlayer_Weapon::IsAttackAnim()
{
	if (
		PLAYER_ATK_LIGHT_01 == *m_pPlayerAnim ||
		PLAYER_ATK_LIGHT_02 == *m_pPlayerAnim ||
		PLAYER_ATK_LIGHT_03 == *m_pPlayerAnim ||
		PLAYER_ATK_LIGHT_04 == *m_pPlayerAnim ||
		PLAYER_ATK_POWER_01 == *m_pPlayerAnim ||
		PLAYER_ATK_POWER_02 == *m_pPlayerAnim ||
		PLAYER_ATK_RUN == *m_pPlayerAnim)
	{
		return true;
	}

	return false;
}

CPlayer_Weapon* CPlayer_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Weapon* pInstance = new CPlayer_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_Weapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Weapon::Clone(void* pArg)
{
	CPlayer_Weapon* pInstance = new CPlayer_Weapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CPlayer_Weapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pSkillColliderCom);
}
