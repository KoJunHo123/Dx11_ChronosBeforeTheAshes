#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
END


BEGIN(Client)
class CPuzzle_InterColl final : public CPartObject
{
public:
	typedef struct : CPartObject::PARTOBJ_DESC
	{
		_bool* pPuzzleReplace = { nullptr };
	}PART_INTERCOLL_DESC;
private:
	CPuzzle_InterColl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPuzzle_InterColl(const CPuzzle_InterColl& Prototype);
	virtual ~CPuzzle_InterColl() = default;

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

	_bool m_bPuzzleSolve = { false };

	class CInventory* m_pInventory = { nullptr };
	_bool* m_pPuzzleReplace = { nullptr };
private:
	HRESULT Ready_Components();

public:
	static CPuzzle_InterColl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
