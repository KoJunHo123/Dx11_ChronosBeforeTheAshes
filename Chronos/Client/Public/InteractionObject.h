#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CInteractionObject abstract : public CGameObject
{
public:
	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos;
		_float3 vScale;
		_float3 vRotation;

		_float3 vExtents;
	}INTERACTION_OBJECT_DESC;
protected:
	CInteractionObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteractionObject(const CInteractionObject& Prototype);
	virtual ~CInteractionObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval);

private:
	class CCollider* m_pColliderCom = { nullptr };

private:
	HRESULT Ready_Components(_float3 vExtents);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END