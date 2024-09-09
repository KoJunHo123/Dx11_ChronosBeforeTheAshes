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

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	HRESULT Priority_Update(_float fTimeDelta);
	HRESULT Update(_float fTimeDelta);
	HRESULT Late_Update(_float fTimeDelta);

	class CComponent* Find_Component(const _wstring& strComponentTag, _uint iIndex);
	void Release_Object();
	void Clear();

private:
	list<class CGameObject*>			m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END