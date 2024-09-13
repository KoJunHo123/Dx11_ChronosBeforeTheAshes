#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

BEGIN(Client)
class CTeleport_Container final : public CContainerObject
{
private:
	CTeleport_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTeleport_Container(const CTeleport_Container& Prototype);
	virtual ~CTeleport_Container() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_PartObject();

public:
	static CTeleport_Container* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
