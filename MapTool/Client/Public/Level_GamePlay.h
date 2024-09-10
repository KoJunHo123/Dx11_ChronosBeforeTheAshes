#pragma once

#include "Client_Defines.h"


#include "Level.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	typedef struct
	{
		_bool isPick = { false };
		_vector vPickPos = {};
	}PICKING_CHECK;
	
	enum ADD_TYPE{ LAYER_MONSTER, LAYER_CELL, LAYER_INTERACTION, LAYER_END };
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Layer_Camera();	
	HRESULT Ready_Layer_BackGround();
	
private:
	PICKING_CHECK IsPicking_Labarynth();
	PICKING_CHECK IsPicking_Interaction();

	HRESULT Add_Monster(_vector vPos, _int iCellIndex);
	HRESULT Add_Cell();
	HRESULT Add_Interaction(_vector vPos);

	HRESULT Save_Monsters();
	HRESULT Load_Monsters();

	HRESULT Save_Cells();
	HRESULT Load_Cells();

	HRESULT Save_Interaction();
	HRESULT Load_Interaction();

private:
	// «ˆ¿Á º±≈√«— ∞¥√º ¿Œµ¶Ω∫..?
	_float3 m_vScale = {};
	_float3 m_vRotation = {};

	_float4 m_vCheckPos{};
	_int m_iObjectLayer = { LAYER_END };

	vector<_float3> m_Points;

	_float m_fOffset = { 0.f };

	class CNavigation* m_pNavigation = { nullptr };

	_bool m_bDelete_PickingCell = { false };
	_bool m_bLink_Teleport = { false };

	vector<class CTeleport*> Teleports;
public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
