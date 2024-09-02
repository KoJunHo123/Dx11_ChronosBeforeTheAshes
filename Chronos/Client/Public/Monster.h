#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CMonster abstract : public CContainerObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos;
		_float3 vScale;
		_float3 vRotation;

		_int iStartCellIndex;
	}MONSTER_DESC;

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_uint m_iCurrentAnimationIndex = { 0 };

	class CNavigation* m_pNavigationCom = { nullptr };

protected:
	virtual HRESULT Ready_Components(_int iStartCellIndex);

public:
	
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END