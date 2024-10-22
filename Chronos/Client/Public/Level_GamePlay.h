#pragma once

#include "Client_Defines.h"


#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:

private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize(_uint iLevelIndex) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_bLevelStart = { false };
	class CPlayer* m_pPlayer = { nullptr };

	_bool m_bYellow = { false };
	_bool m_bPupple = { false };
	_bool m_bRed = { false };

	_float m_fTime = { 0.f };
	_bool m_bChangeBGM = { false };
	_float m_fVolume = { 0.f };

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_Inventory();
	HRESULT Ready_Layer_Player();
	HRESULT Ready_Layer_Monster();
	HRESULT Ready_Layer_Paticle();

	HRESULT Ready_Layer_Interaction();
	HRESULT Ready_Layer_Puzzle();
	HRESULT Ready_Layer_Teleport();
	HRESULT Ready_Layer_Pedestal();
	HRESULT Ready_Layer_Waypoint();
	HRESULT Ready_Layer_DoorLock();

	HRESULT Ready_Layer_UI();

	HRESULT Ready_Layer_Monster_Yellow();
	HRESULT Ready_Layer_Monster_Pupple();
	HRESULT Ready_Layer_Monster_Red();

	HRESULT Ready_PoolingLayer_FloorChunk();

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelIndex);
	virtual void Free() override;
};

END
