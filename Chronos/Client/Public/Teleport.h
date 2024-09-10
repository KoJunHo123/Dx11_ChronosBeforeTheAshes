#pragma once

#include "InteractionObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)
class CTeleport final : public CInteractionObject
{
public:
	typedef struct : CInteractionObject::INTERACTION_OBJECT_DESC
	{
		_float3 vTeleportPos;
	}TELEPORT_DESC;
protected:
	CTeleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTeleport(const CTeleport& Prototype);
	virtual ~CTeleport() = default;

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
	class CModel* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };

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
