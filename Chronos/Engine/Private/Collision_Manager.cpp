#include "Collision_Manager.h"
#include "GameInstance.h"

CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Initialize()
{
	return S_OK;
}

void CCollision_Manager::Update()
{
	for (size_t i = 0; i < m_CollisionKeysFirst.size(); ++i)
	{
		if (m_ColliderLayers.find(m_CollisionKeysFirst[i]) == m_ColliderLayers.end())
			continue;

		if (m_ColliderLayers.find(m_CollisionKeysSecond[i]) == m_ColliderLayers.end())
			continue;

		for (auto& Dest : m_ColliderLayers.at(m_CollisionKeysFirst[i]))
		{
			for (auto& Sour : m_ColliderLayers.at(m_CollisionKeysSecond[i]))
			{
				if (Dest == Sour)
					continue;

				Dest->Intersect(m_CollisionKeysSecond[i], Sour);
				Sour->Intersect(m_CollisionKeysFirst[i], Dest);
			}
		}
	}
}

void CCollision_Manager::Add_Collider_OnLayers(const _wstring strCollisionKey, class CCollider* pCollider)
{
	if (m_ColliderLayers.end() == m_ColliderLayers.find(strCollisionKey))
	{
		vector<CCollider*> pColliders;
		m_ColliderLayers.emplace(strCollisionKey, pColliders);
	}
	
	m_ColliderLayers.at(strCollisionKey).emplace_back(pCollider);
	Safe_AddRef(pCollider);
}

CCollision_Manager* CCollision_Manager::Create()
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Create Failed : CCollision_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollision_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_ColliderLayers)
	{
		for (auto& elem : Pair.second)
			Safe_Release(elem);
	}
}
