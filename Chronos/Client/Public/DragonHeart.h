#pragma once

#include "Client_Defines.h"
#include "Item.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
END

BEGIN(Client)
class CDragonHeart final : public CItem
{
public:
	typedef struct : CItem::ITEM_DESC
	{

	}DRAGONHEART_DESC;

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
	virtual HRESULT Render(const _float4x4& WorldMatrix);

public:
	virtual void Add_Item() override;
	virtual _bool Use_Item(class CPlayer* pPlayer) override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

	_uint m_iMaxCount = { 4 };

private:
	HRESULT Ready_Components();

public:
	static CDragonHeart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};
END
