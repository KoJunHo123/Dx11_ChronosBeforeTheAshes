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
		for (auto& iterSour = m_ColliderLayers.at(m_CollisionKeysFirst[i]).begin(); iterSour != m_ColliderLayers.at(m_CollisionKeysFirst[i]).end(); ++iterSour)
		{
			if (nullptr == *iterSour)
			{
				m_ColliderLayers.at(m_CollisionKeysFirst[i]).erase(iterSour);
				continue;
			}

			for (auto& iterDest = m_ColliderLayers.at(m_CollisionKeysSecond[i]).begin(); iterDest != m_ColliderLayers.at(m_CollisionKeysSecond[i]).end(); ++iterDest)
			{
				if (*iterSour == *iterDest)
					continue;

				if (nullptr == *iterDest)
				{
					m_ColliderLayers.at(m_CollisionKeysSecond[i]).erase(iterDest);
					continue;
				}

				(*iterSour)->Intersect(m_CollisionKeysSecond[i], *iterDest);
				(*iterDest)->Intersect(m_CollisionKeysFirst[i], *iterSour);
			}
		}
	}
}

void CCollision_Manager::Add_Collider_OnLayers(const _wstring strCollisionKey, class CCollider* pCollider)
{
	if (m_ColliderLayers.end() == m_ColliderLayers.find(strCollisionKey))
	{
		list<CCollider*> pColliders;
		m_ColliderLayers.emplace(strCollisionKey, pColliders);
	}
	
	m_ColliderLayers.at(strCollisionKey).emplace_back(pCollider);
}

void CCollision_Manager::Erase_Collider(const _wstring strCollisionKey, CCollider* pCollider)
{
	auto& PairIter = m_ColliderLayers.find(strCollisionKey);

	if (m_ColliderLayers.end() == PairIter)
		return;

	for (auto iter = (*PairIter).second.begin(); iter != (*PairIter).second.end();)
	{
		if (pCollider == *iter)
		{
			iter = (*PairIter).second.erase(iter);
		}
		else
		{
			++iter;
		}
	}
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

}
