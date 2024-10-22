#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CDoorLock_InterColl final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_float fOffset;
		_bool* pUnLock;
	}INTERCOLL_DESC;
private:
	CDoorLock_InterColl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDoorLock_InterColl(const CDoorLock_InterColl& Prototype);
	virtual ~CDoorLock_InterColl() = default;

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
	_bool* m_pUnLock;

private:
	HRESULT Ready_Components(_float fOffset);
public:
	static CDoorLock_InterColl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};
END
