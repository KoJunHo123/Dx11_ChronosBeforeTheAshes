#pragma once

#include "Monster.h"

BEGIN(Client)
class CBoss_Lab final : public CMonster
{
public:
	enum PARTID { PART_BODY, PART_EFFECT, PART_END };
	enum STATE { STATE_ATTACK, STATE_IDLE, STATE_JUMP, STATE_STUN, STATE_TELEPORT, STATE_MOVE, STATE_END };

private:
	CBoss_Lab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Lab(const CBoss_Lab& Prototype);
	virtual ~CBoss_Lab() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	STATE m_eState = { STATE_END };

	CTransform* m_pPlayerTransformCom = { nullptr };

	_bool m_bMotionLock = { false };
	_bool m_Test = false;

	// 파츠랑 공유하는 변수
	BOSS_ANIM m_eBossAnim = { BOSS_ANIM_END };
	_bool m_isFinished = { false };
	_float m_fSpeed = { 10.f };
	// 플레이어와의 거리.
	// 근접 공격
	_float m_fMeleeAttack_Dist = { 10.f };
	// 약간 떨어진 거리
	_float m_fNearAttack_Dist = { 20.f };
	// 중거리
	_float m_fMidRangeAttack_Dist = { 30.f };
	// 원거리
	_float m_fLongRangeAttack_Dist = { 40.f };

	// 공격 딜레이
	_float m_fAttackDelay = { 0.f };
	_float m_fChargeTime = { 0.f };

	// 텔레포트 공격 횟수
	_uint m_iTeleportAttack_Count = { 0 };
	_float m_fTeleportDelay = { 0.f };

private:
	virtual HRESULT Ready_Components();
	HRESULT Ready_PartObjects();

	void Attack_Type_Desc(_float fDistance);
	
	void Finished_Animation_Control();
	void Teleport_Control();

	void Teleport_Progress(_float fTimeDelta);

	void TurnTo_Player(_float fRadian, _float fTimeDelta);

public:
	static CBoss_Lab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
