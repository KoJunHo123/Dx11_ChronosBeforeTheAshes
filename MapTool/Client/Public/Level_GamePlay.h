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
	
	enum ADD_TYPE{ LAYER_PLAYER, LAYER_MONSTER, CELL, WALLCELL, COLLIDER, LAYER_INTERACTION, LAYER_END };

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
	PICKING_CHECK IsPicking();
	PICKING_CHECK IsPicking_Labarynth();

	HRESULT Add_Monster(_vector vPos, _int iCellIndex);
	HRESULT Add_Player(_vector vPos, _int iCellIndex);
	HRESULT Add_Cell();
	HRESULT Add_Collider(_fvector vPos);
	
	HRESULT Add_Object(_vector vPos);

	HRESULT Save_Monsters();
	HRESULT Load_Monsters();

	HRESULT Save_Player();
	HRESULT Load_Player();

	HRESULT Save_Cells();
	HRESULT Load_Cells();

	HRESULT Save_Objects();
	HRESULT Load_Objects();

private:
	// «ˆ¿Á º±≈√«— ∞¥√º ¿Œµ¶Ω∫..?
	_float3 m_vScale = {};
	_float3 m_vRotation = {};

	_float3 m_vExtents = {};

	vector<_wstring> m_PrototypeKeys;

	_float4 m_vCheckPos{};
	_int m_iObjectLayer = { CELL };
	_int m_iPickingObject = { 0 };
	_int m_iPickingEffect = { 0 };

	vector<_float3> m_Points;

	_float m_fOffset = { 0.f };

	class CNavigation* m_pNavigation = { nullptr };

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
