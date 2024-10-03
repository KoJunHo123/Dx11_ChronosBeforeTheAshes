#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTransform;
END

BEGIN(Client)
class CPedestal_Item final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_float fOffset = { 0.f };
		_float3 vRotation = {};
		class CItem** ppItem = { nullptr };
	}PEDESTAL_ITEM_DESC;
private:
	CPedestal_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPedestal_Item(const CPedestal_Item& Prototype);
	virtual ~CPedestal_Item() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CItem** m_ppItem = { nullptr };

private:
	_float m_fOffset = { 0.f };
	_float m_fSpeed = { 0.f };
	_bool m_bUp = { false };

private:
	HRESULT Ready_Components();

public:
	static CPedestal_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();

};
END
