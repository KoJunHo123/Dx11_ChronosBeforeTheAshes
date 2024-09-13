#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)
class CTeleport final : public CPartObject
{
public:
	typedef struct : CPartObject::PARTOBJ_DESC
	{
		_float3 vPos;
		_float3 vTeleportPos;
	}TELEPORT_DESC;
private:
	CTeleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTeleport(const CTeleport& Prototype);
	virtual ~CTeleport() = default;

public:
	void Set_TeleportPos(_fvector vTeleportPos) {
		XMStoreFloat3(&m_vTeleportPos, vTeleportPos);
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval);

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CCollider* m_pColliderCom = { nullptr };

private:
	_float3 m_vTeleportPos = {};

private:
	HRESULT Ready_Components();

public:
	static CTeleport* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
