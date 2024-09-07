#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CMoedl;
class CShader;
END


BEGIN(Client)
class CLab_Troll_Weapon final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };
	}WEAPON_DESC;

private:
	CLab_Troll_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLab_Troll_Weapon(const CLab_Troll_Weapon& Prototype);
	virtual ~CLab_Troll_Weapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CLab_Troll_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CLab_Troll_Weapon* Clone(void* pArg) override;
	virtual void Free() override;
};
END
