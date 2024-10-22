#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CWayPoint final : public CContainerObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos;
	}WAYPOINT_DESC;

	enum PART { PART_INTERCOLL, PART_EFFECT, PART_END};

private:
	CWayPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWayPoint(const CWayPoint& Prototype);
	virtual ~CWayPoint() = default;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
public:
	virtual void Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval) override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CCollider* m_pColliderCom = { nullptr };

	class CTransform* m_pPlayerTransformCom = { nullptr };

	_bool m_bIntersect = { false };

	_float m_fVolume = { 0.f };

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObject();

public:
	static CWayPoint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;

};
END
