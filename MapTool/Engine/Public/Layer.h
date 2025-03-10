#pragma once

#include "Base.h"

/* 객체들을 보관한다. */

BEGIN(Engine)

class ENGINE_DLL CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	HRESULT Priority_Update(_float fTimeDelta);
	HRESULT Update(_float fTimeDelta);
	HRESULT Late_Update(_float fTimeDelta);

	class CGameObject* Get_GameObject(_uint iIndex);

public:
	class CComponent* Find_Component(const _wstring& strComponentTag, _uint iIndex);
	class CGameObject* Find_GameObject(_uint iIndex);
	HRESULT Save_GameObjects(ofstream* pOutFile);
	void Release_Object();
	void Clear();

private:
	list<class CGameObject*>			m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END