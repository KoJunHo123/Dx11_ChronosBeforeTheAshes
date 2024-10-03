#pragma once

#include "ContainerObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)
class CPedestal final : public CContainerObject
{
public:
	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos = {};
		_float3 vRotation = {};
		_wstring strItemTag = L"";
		class CItem* pItem = { nullptr };
	}PEDESTAL_DESC;

	enum PART { PART_ITEM, PART_INTERCOLL, PART_END };

protected:
	CPedestal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPedestal(const CPedestal& Prototype);
	virtual ~CPedestal() = default;

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
	class CModel* m_pModelCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CCollider* m_pColliderCom = { nullptr };
	class CItem* m_pItem = { nullptr };


private:
	_bool m_bItemUsed = { false };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Parts(const _wstring strItemTag, _float3 vRotation);
	
public:
	static CPedestal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END
