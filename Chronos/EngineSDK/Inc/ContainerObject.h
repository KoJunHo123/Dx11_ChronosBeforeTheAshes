#pragma once

#include "GameObject.h"

BEGIN(Engine)
class ENGINE_DLL CContainerObject  : public CGameObject
{
protected:
	CContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CContainerObject(const CContainerObject& Prototype);
	virtual ~CContainerObject() = default;

public:
	virtual class CComponent* Find_PartComponent(const _wstring& strComponentTag, _uint iPartObjIndex);
	virtual class CPartObject* Get_PartObject(_uint iIndex);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	vector<class CPartObject*>			m_Parts;

protected:
	HRESULT Add_PartObject(_uint iPartID, const _wstring& strPrototypeTag, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual CGameObject* Pooling() = 0;
	virtual void Free() override;
	virtual void Return();
};

END