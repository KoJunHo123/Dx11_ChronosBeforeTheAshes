#pragma once

#include "PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CNavigation;
class CModel;
class CShader;
END

BEGIN(Client)
class CLab_Troll_Body final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		class CTransform* pConstruct_TransformCom = { nullptr };
		class CNavigation* pNavigationCom = { nullptr };

		const TROLL_ANIM* pTrollAnim = { nullptr };
		_bool* pIsFinished = { nullptr };
	}BODY_DESC;

private:
	CLab_Troll_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLab_Troll_Body(const CLab_Troll_Body& Prototype);
	virtual ~CLab_Troll_Body() = default;

public:
	const _float4x4* Get_BoneMatrix_Ptr(const _char* pBoneName) const;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CTransform* m_pConstruct_TransformCom = { nullptr };
	class CNavigation* m_pNavigationCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

private:
	const TROLL_ANIM* m_pTrollAnim = { nullptr };
	_bool* m_pIsFinished = { nullptr };
	
private:
	HRESULT Ready_Components();
private:
	void Play_Animation(_float fTimeDelta);

public:
	static CLab_Troll_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END
