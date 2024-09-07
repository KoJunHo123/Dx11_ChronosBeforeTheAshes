#include "ColliderLayer.h"
#include "GameInstance.h"

CColliderLayer::CColliderLayer()
{
}

HRESULT CColliderLayer::Add_Collider(CCollider* pCollider)
{


	return S_OK;
}

HRESULT CColliderLayer::Update(_wstring strDestCollider, _wstring strSourCollider, CColliderLayer* pColliderLayer)
{
	for (auto& Dest_Collider : m_Colliders)
	{
		for (auto& Sour_Collider : pColliderLayer->m_Colliders)
		{
			Dest_Collider->Intersect(strSourCollider, Sour_Collider);
			Sour_Collider->Intersect(strDestCollider, Dest_Collider);
		}
	}

	return S_OK;
}

CColliderLayer* CColliderLayer::Create()
{
	return new CColliderLayer;
}

void CColliderLayer::Free()
{
	__super::Free();

	for (auto& Collider : m_Colliders)
	{
		Safe_Release(Collider);
	}
}
