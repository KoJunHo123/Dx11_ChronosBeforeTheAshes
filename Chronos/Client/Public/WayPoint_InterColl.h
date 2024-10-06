#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CWayPoint_InterColl final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_float fOffset;
		_bool* pIntersect;
	}INTERCOLL_DESC;
private:
	CWayPoint_InterColl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWayPoint_InterColl(const CWayPoint_InterColl& Prototype);
	virtual ~CWayPoint_InterColl() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
public:
	virtual void Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval) override;

private:
	class CCollider* m_pColliderCom = { nullptr };
	_bool* m_pIntersect = { nullptr };
private:
	HRESULT Ready_Components(_float fOffset);
public:
	static CWayPoint_InterColl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
