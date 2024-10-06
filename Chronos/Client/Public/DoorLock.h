#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END


BEGIN(Client)
class CDoorLock final : public CContainerObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos;
	}DOORLOCK_DESC;

	enum PART { PART_INTERCOLL, PART_END};

private:
	CDoorLock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDoorLock(const CDoorLock& Prototype);
	virtual ~CDoorLock() = default;
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


private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObject();

public:
	static CDoorLock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END
