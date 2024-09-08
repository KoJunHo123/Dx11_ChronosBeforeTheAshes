#include "stdafx.h"
#include "Boss_Lab_Attack.h"
#include "GameInstance.h"

#include "Player.h"

CBoss_Lab_Attack::CBoss_Lab_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CBoss_Lab_Attack::CBoss_Lab_Attack(const CBoss_Lab_Attack& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CBoss_Lab_Attack::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Lab_Attack::Initialize(void* pArg)
{
	ATTACK_DESC* pDesc = static_cast<ATTACK_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketMatrix;
	m_iDamage = pDesc->iDamage;
	m_pAttackActive = pDesc->pAttackActive;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->vExtents, pDesc->vCenter, pDesc->vAngles)))
		return E_FAIL;

	return S_OK;
}

_uint CBoss_Lab_Attack::Priority_Update(_float fTimeDelta)
{
	m_pColliderCom->Set_OnCollision(*m_pAttackActive);

	return OBJ_NOEVENT;
}

void CBoss_Lab_Attack::Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	// ??
	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	// 내 위치 * 붙여야 할 뼈의 위치 * 플레이어 위치 -> 플레이어의 위치에서 붙여야 할 뼈의 위치.
	// -> 셰이더에서 해주던 뼈 * 월드를 여기서 하는 거.
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CBoss_Lab_Attack::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBoss_Lab_Attack::Render()
{
#ifdef  _DEBUG
	m_pColliderCom->Render();
#endif //  _DEBUG
	return S_OK;
}

void CBoss_Lab_Attack::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
	if (true == *m_pAttackActive && TEXT("Coll_Player") == strColliderTag)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(pCollisionObject);
		pPlayer->Be_Damaged(m_iDamage, XMLoadFloat4x4(m_pParentMatrix).r[3]);
	}
}

HRESULT CBoss_Lab_Attack::Ready_Components(_float3 vExtents, _float3 vCenter, _float3 vAngles)
{
	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderOBBDesc{};
	ColliderOBBDesc.vExtents = vExtents;
	ColliderOBBDesc.vCenter = vCenter;
	ColliderOBBDesc.vAngles = vAngles;

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.pOwnerObject = this;
	ColliderDesc.pBoundingDesc = &ColliderOBBDesc;
	ColliderDesc.bCollisionOnce = true;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pGameInstance->Add_Collider_OnLayers(TEXT("Coll_Monster_Attack"), m_pColliderCom);

	return S_OK;
}

CBoss_Lab_Attack* CBoss_Lab_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Lab_Attack* pInstance = new CBoss_Lab_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CBoss_Lab_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Lab_Attack::Clone(void* pArg)
{
	CBoss_Lab_Attack* pInstance = new CBoss_Lab_Attack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CBoss_Lab_Attack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Lab_Attack::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
