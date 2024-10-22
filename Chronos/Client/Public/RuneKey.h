#pragma once

#include "Item.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CRuneKey final : public CItem
{
public:
	typedef struct : CItem::ITEM_DESC
	{

	}RUNEKYE_DESC;

private:
	CRuneKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRuneKey(const CRuneKey& Prototype);
	virtual ~CRuneKey() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(const _float4x4& WorldMatrix);

public:
	virtual _bool Use_Item(class CPlayer* pPlayer) override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CRuneKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free();
};
END
