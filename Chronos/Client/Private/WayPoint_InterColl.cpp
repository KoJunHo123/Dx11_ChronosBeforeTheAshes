#include "stdafx.h"
#include "WayPoint_InterColl.h"
#include "GameInstance.h"

#include "Player.h"
#include "Particle_Save.h"

CWayPoint_InterColl::CWayPoint_InterColl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CWayPoint_InterColl::CWayPoint_InterColl(const CWayPoint_InterColl& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CWayPoint_InterColl::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWayPoint_InterColl::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	INTERCOLL_DESC* pDesc = static_cast<INTERCOLL_DESC*>(pArg);
	
	if (FAILED(Ready_Components(pDesc->fOffset)))
		return E_FAIL;

	m_pColliderCom->Set_OnCollision(true);

	m_pIntersect = pDesc->pIntersect;
	return S_OK;
}

_uint CWayPoint_InterColl::Priority_Update(_float fTimeDelta)
{
	*m_pIntersect = true;

	return OBJ_NOEVENT;
}

void CWayPoint_InterColl::Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CWayPoint_InterColl::Late_Update(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CWayPoint_InterColl::Render()
{
	return S_OK;
}

void CWayPoint_InterColl::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
	if (TEXT("Coll_Player") == strColliderTag && m_pGameInstance->Get_DIKeyState_Down(DIKEYBOARD_E))
	{
		_vector vPos = XMLoadFloat4x4(&m_WorldMatrix).r[3];
		static_cast<CPlayer*>(pCollisionObject)->Set_SavePos(vPos);
		*m_pIntersect = false;


		CParticle_Save::PARTICLE_SAVE_DESC desc = {};
		XMStoreFloat3(&desc.vPos, XMLoadFloat4x4(&m_WorldMatrix).r[3]);

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Save"), &desc);

		SOUND_DESC SoundDecs = {};
		SoundDecs.fVolume = 1.f;

		m_pGameInstance->StopSound(SOUND_WAYPOINT_INTERACT);
		m_pGameInstance->SoundPlay(TEXT("Waypoint_Stone_Start_HandsOn.ogg"), SOUND_WAYPOINT_INTERACT, SoundDecs);
	}
}

HRESULT CWayPoint_InterColl::Ready_Components(_float fOffset)
{
	CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc = {};
	BoundingDesc.vCenter = { 0.f, fOffset, 0.f };
	BoundingDesc.vExtents = { 5.f, 5.f, 5.f };

	CCollider::COLLIDER_DESC CollDesc = {};
	CollDesc.bCollisionOnce = false;
	CollDesc.pOwnerObject = this;
	CollDesc.strColliderTag = TEXT("Coll_Interaction");
	CollDesc.pBoundingDesc = &BoundingDesc;

	/* FOR.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollDesc)))
		return E_FAIL;

	return S_OK;
}

CWayPoint_InterColl* CWayPoint_InterColl::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWayPoint_InterColl* pInstance = new CWayPoint_InterColl(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CWayPoint_InterColl"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWayPoint_InterColl::Clone(void* pArg)
{
	CWayPoint_InterColl* pInstance = new CWayPoint_InterColl(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Create Failed : CWayPoint_InterColl"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWayPoint_InterColl::Pooling()
{
	return new CWayPoint_InterColl(*this);
}

void CWayPoint_InterColl::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
}
