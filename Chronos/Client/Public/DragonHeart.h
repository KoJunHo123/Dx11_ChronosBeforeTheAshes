#pragma once

#include "Client_Defines.h"
#include "Item.h"

BEGIN(Client)
class CDragonHeart final : public CItem
{
private:
	CDragonHeart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDragonHeart(const CDragonHeart& Prototype);
	virtual ~CDragonHeart() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual _bool Use_Item() override;

private:
	HRESULT Ready_Components();

public:
	static CDragonHeart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};
END
