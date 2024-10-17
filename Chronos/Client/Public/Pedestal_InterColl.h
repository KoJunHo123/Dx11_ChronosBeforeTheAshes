#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
END


BEGIN(Client)
class CPedestal_InterColl final : public CPartObject
{
public:
	typedef struct : CPartObject::PARTOBJ_DESC
	{
		_wstring strItemTag = L"";
		class CItem** ppItem = { nullptr };
	}PART_INTERCOLL_DESC;

private:
	CPedestal_InterColl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPedestal_InterColl(const CPedestal_InterColl& Prototype);
	virtual ~CPedestal_InterColl() = default;

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
	class CItem** m_ppItem = { nullptr };

	_wstring m_strItemTag = L"";
	_bool* m_pNonItem = { false };
private:
	HRESULT Ready_Components();

public:
	static CPedestal_InterColl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
