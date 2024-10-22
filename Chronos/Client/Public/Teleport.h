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
		_float4 vColor;
		_bool bActive;
		CTeleport* pTeleport;
	}TELEPORT_DESC;
private:
	CTeleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTeleport(const CTeleport& Prototype);
	virtual ~CTeleport() = default;

public:
	void Set_Teleport(CTeleport* pTeleport) {
		m_pTeleport = pTeleport;
	}

	void Set_Active() {
		m_bActive = true;
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
	CTeleport* m_pTeleport = { nullptr };
	_float4 m_vColor = {};

	_bool m_bActive = { false };

private:
	HRESULT Ready_Components();

public:
	static CTeleport* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};
END
