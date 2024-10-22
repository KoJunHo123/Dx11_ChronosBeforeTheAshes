#pragma once

#include "Base.h"

/* 객체들을 보관한다. */

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	list<class CGameObject*> Get_GameObjects() {
		return m_GameObjects;
	}

	size_t Get_ObjectSize() {
		return m_GameObjects.size();
	}

	CLayer* Get_PoolingLayer() {
		return m_pPoolingLayer;
	}

	void Set_PoolingLayer(CLayer* pPoolingLayer) {
		if(nullptr == m_pPoolingLayer)
		{
			m_pPoolingLayer = pPoolingLayer;
			Safe_AddRef(m_pPoolingLayer);
		}
	}

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	HRESULT Priority_Update(_float fTimeDelta);
	HRESULT Update(_float fTimeDelta);
	HRESULT Late_Update(_float fTimeDelta);

	class CGameObject* Get_GameObject(_uint iObjectIndex);
	class CGameObject* Get_FrontGameObject();

	class CComponent* Find_Component(const _wstring& strComponentTag, _uint iIndex);
	class CComponent* Find_PartComponent(const _wstring& strComponentTag, _uint iIndex, _uint iPartObjIndex);

	void Release_Object();
	void Release_Object(_uint iIndex);
	void Clear();

private:
	list<class CGameObject*>			m_GameObjects;

	CLayer* m_pPoolingLayer = { nullptr };

public:
	static CLayer* Create();
	virtual void Free() override;
};

END