#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CBoss_Lab_Body final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		class CTransform* pBossTransformCom = { nullptr };
		class CNavigation* pNavigationCom = { nullptr };

		const BOSS_ANIM* pBossAnim = { nullptr };
		_bool* pIsFinished = { nullptr };
	}BODY_DESC;

private:
	CBoss_Lab_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Lab_Body(const CBoss_Lab_Body& Prototype);
	virtual ~CBoss_Lab_Body() = default;

public:
	const _float4x4* Get_BoneMatrix_Ptr(const _char* pBoneName) const;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CTransform* m_pBossTransformCom = { nullptr };
	class CNavigation* m_pNavigationCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

private:
	const BOSS_ANIM*	m_pBossAnim = { nullptr };
	_bool*				m_pIsFinished = { nullptr };

private:
	HRESULT Ready_Components();
private:
	void Play_Animation(_float fTimeDelta);

public:
	static CBoss_Lab_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
