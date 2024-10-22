#include "..\Public\Layer.h"

#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.emplace_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end(); )
	{
		_uint	iResult = (*iter)->Priority_Update(fTimeDelta);

		if (OBJ_DEAD == iResult)
		{
			Safe_Release(*iter);
			iter = m_GameObjects.erase(iter);
		}
		else if (OBJ_RETURN == iResult && nullptr != m_pPoolingLayer)
		{
			(*iter)->Return();
			m_pPoolingLayer->Add_GameObject(*iter);
			iter = m_GameObjects.erase(iter);
		}
		else
		{
			iter++;
		}
	}

	return S_OK;
}

HRESULT CLayer::Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject->Update(fTimeDelta);

	return S_OK;
}

HRESULT CLayer::Late_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject->Late_Update(fTimeDelta);

	return S_OK;
}

CGameObject* CLayer::Get_GameObject(_uint iObjectIndex)
{
	_uint iIndex = { 0 };
	for (auto& GameObject : m_GameObjects)
	{
		if (iObjectIndex == iIndex)
			return GameObject;

		++iIndex;
	}

	return nullptr;
}

CGameObject* CLayer::Get_FrontGameObject()
{
	auto& iter = m_GameObjects.begin();

	if (iter == m_GameObjects.end())
		return nullptr;

	CGameObject* pGameObject = *iter;
	m_GameObjects.erase(iter);

	return pGameObject;
}


CComponent * CLayer::Find_Component(const _wstring & strComponentTag, _uint iIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Find_Component(strComponentTag);	
}

CComponent* CLayer::Find_PartComponent(const _wstring& strComponentTag, _uint iIndex, _uint iPartObjIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Find_PartComponent(strComponentTag, iPartObjIndex);
}


void CLayer::Release_Object()
{
	if (true == m_GameObjects.empty())
		return;

	Safe_Release(m_GameObjects.back());
	m_GameObjects.pop_back();
}

void CLayer::Release_Object(_uint iIndex)
{
	if (true == m_GameObjects.empty())
		return;

	auto& iter = m_GameObjects.begin();
	for (_uint i = 0; i < m_GameObjects.size(); ++i)
	{
		if (iIndex == i)
		{
			Safe_Release(*iter);
			iter = m_GameObjects.erase(iter);
			return;
		}
		else
			++iter;
	}
}

void CLayer::Clear()
{
	for (auto& pGameObject : m_GameObjects)
	{
		pGameObject->Set_Dead(true);
		Safe_Release(pGameObject);
	}
	m_GameObjects.clear();
}


CLayer * CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);
	m_GameObjects.clear();

	Safe_Release(m_pPoolingLayer);
}

