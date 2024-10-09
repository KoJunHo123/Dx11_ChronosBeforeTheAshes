#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

#include "Player_Part.h"

BEGIN(Engine)
class CFSM;
class CNavigation;
class CCollider;
class CState;
class CCamera;
class CVIBuffer_Trail_TwoPoint_Instance;
END

BEGIN(Client)
class CPlayer final : public CContainerObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos;
		_float3 vScale;
		_float3 vRotation;
		_int iStartCellIndex;
	}PLAYER_DESC;

	enum PLAYER_STATE{ STATE_MOVE, STATE_ATTACK, STATE_JUMP, STATE_BLOCK, STATE_IMPACT, STATE_ACTION, STATE_END};
	enum PLAYER_PART{ PART_BODY, PART_WEAPON, PART_SHIELD, PART_ITEM, PART_EFFECT, PART_END };
	enum PLAYER_SKILL { SKILL_RED, SKILL_PUPPLE, SKILL_END };

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual void Set_Position(_vector vPos) {
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		m_pNavigationCom->Set_CurrentCellIndex_ByPos(vPos);
	}

	void Recovery_HP() {
		m_fHP = m_fMaxHP;
	}
	void Acquire_Skill(_uint iSkillIndex) {
		m_HaveSkill[iSkillIndex] = true;
	}

	_uint Get_Skill() {
		return m_eCurrentSkill;
	}

	_float Get_HP() {
		return m_fHP;
	}

	_float Get_MaxHP() {
		return m_fMaxHP;
	}

	_float Get_Stamina() {
		return m_fStamina;
	}

	_float Get_MaxStamina() {
		return m_fMaxStamina;
	}

	_float Get_SkillGage() {
		return m_fSkillGage;
	}

	_float Get_MaxSkillGage() {
		return m_fMaxSkillGage;
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
	_bool Be_Damaged(_float fDamage, _fvector vAttackPos);
	void Set_SavePos(_fvector vPos);
	_bool Use_Runekey(_fvector vPos, _fvector vLookAt);
	void Start_Teleport(_fvector vPos);

public:
	class CFSM* m_pFSM = { nullptr };
	class CNavigation* m_pNavigationCom = { nullptr };
	class CCollider* m_pColliderCom = { nullptr };
	class CTexture* m_pNoiseTextureCom = { nullptr };



	// 상태와 파츠가 공유해야 하는 변수들
	PLAYER_ANIM m_ePlayerAnim = { PLAYER_ANIM_END };
	_float m_fSpeed = { 0.f };
	_float m_fStartSpeed = { 0.f };
	_bool m_isFinished = { false };
	_float3 m_vCameraLook = {};

	// 무기랑 공유하는 변수
	_uint m_iKeyFrameIndex = { 0 };

	// 아이템이랑 공유하는 변수
	_bool m_bItemUsed = { false };

	// 체력
	_float m_fHP = { 0 };
	_float m_fMaxHP = { 0 };
	// 스테미너
	_float m_fStamina = { 0 };
	_float m_fMaxStamina = { 0 };
	// 스킬 게이지
	_float m_fSkillGage = { 0 };
	_float m_fMaxSkillGage = { 0 };

	// 닷지시 무적
	_bool m_bNonIntersect = { false };
	
	// 사라지는 거인데 0으로 고정할 예정.
	_float m_fRatio = { 0.f };

	_float3 m_vSavePos = {};

	class CCamera* m_pCurrentCamera = { nullptr };
	_bool m_bRevive = { false };

	_float m_fDeathDelay = { 0.f };
	_float m_fSkillDuration = { 0.f };

	class CInventory* m_pInventory = { nullptr };

	// 현재 플레이어 스킬
	_bool m_HaveSkill[SKILL_END] = {};
	_uint m_eCurrentSkill = { SKILL_RED };
	_bool m_bDrain = { false };

private:
	HRESULT Ready_Components(_int iStartCellIndex);
	HRESULT Ready_States();
	HRESULT Ready_Parts();
	HRESULT Ready_Inventory();

private:
	HRESULT Ready_Body(const CPlayer_Part::PLAYER_PART_DESC& BaseDesc);
	HRESULT Ready_Weapon(const CPlayer_Part::PLAYER_PART_DESC& BaseDesc);
	HRESULT Ready_Shield(const CPlayer_Part::PLAYER_PART_DESC& BaseDesc);
	HRESULT Ready_Item(const CPlayer_Part::PLAYER_PART_DESC& BaseDesc);

private:
	void Anim_Frame_Control();
	HRESULT Add_TrailRevolve();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
