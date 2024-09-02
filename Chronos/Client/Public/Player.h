#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

#include "Player_Part.h"

BEGIN(Engine)
class CFSM;
class CNavigation;
class CState;
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
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	class CFSM* m_pFSM = { nullptr };
	class CNavigation* m_pNavigationCom = { nullptr };

	// 상태와 파츠가 공유해야 하는 변수들
	PLAYER_ANIM m_ePlayerAnim = { PLAYER_ANIM_END };
	_float m_fSpeed = { 0.f };
	_bool m_isFinished = { false };
	_float3 m_vCameraLook = {};

	// 무기랑 공유하는 변수
	_uint m_iKeyFrameIndex = { 0 };

private:
	HRESULT Ready_Components(_int iStartCellIndex);
	HRESULT Ready_States();
	HRESULT Ready_Parts();

private:
	HRESULT Ready_Body(const CPlayer_Part::PLAYER_PART_DESC& BaseDesc);
	HRESULT Ready_Weapon(const CPlayer_Part::PLAYER_PART_DESC& BaseDesc);
	HRESULT Ready_Shield(const CPlayer_Part::PLAYER_PART_DESC& BaseDesc);

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
