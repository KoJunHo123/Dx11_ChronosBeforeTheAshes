#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTransform;
class CTexture;
END

BEGIN(Client)
class CMonster abstract : public CContainerObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vScale;
		_float3 vRotation;

		_int iStartCellIndex;
	}MONSTER_DESC;

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	_float Get_HP() {
		return m_fHP;
	}
	_float Get_MaxHP() {
		return m_fMaxHP;
	}

	_float Get_Offset() {
		return m_fOffset;
	}
	_bool Get_Aggro() {
		return m_bAggro;
	}

	_bool On_Damaged() {
		if (0.f < m_fDamagedDelay)
			return true;
		return false;
	}

	MONSTER_TYPE Get_MonsterType() {
		return m_eMonsterType;
	}


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval) override;

public:
	virtual void Be_Damaged(_float fDamage, _fvector vAttackPos);


protected:
	_uint m_iCurrentAnimationIndex = { 0 };

	class CNavigation* m_pNavigationCom = { nullptr };
	class CTransform* m_pPlayerTransformCom = { nullptr };
	class CTexture* m_pNoiseTextureCom = { nullptr };

	_float m_fSpeed = { 0.f };

	_float m_fHP = { 0.f };
	_float m_fMaxHP = { 0.f };

	_float m_fOffset = { 0.f };

	_float m_fRatio = { 0.f };

	_bool m_bAggro = { false };

	_float m_fDamagedDelay = { 0.f };

	class CUIObject* m_pHPBar = { nullptr };

	_bool m_bStart = { false };

	MONSTER_TYPE m_eMonsterType = { MONSTER_END };
	
	_bool m_bScream = { false };
protected:
	HRESULT Add_MonsterHPBar();

private:
	HRESULT Ready_Components(_int iStartCellIndex);

public:
	virtual CGameObject* Pooling() = 0;
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END