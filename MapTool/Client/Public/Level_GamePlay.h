#pragma once

#include "Client_Defines.h"


#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	typedef struct
	{
		_bool isPick = { false };
		_vector vPickPos = {};
	}PICKING_CHECK;

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

	HRESULT Add_Monster(_vector vPos);

	HRESULT Save_Monsters();
	HRESULT Load_Monsters();

private:
	// «ˆ¿Á º±≈√«— ∞¥√º ¿Œµ¶Ω∫..?
	_float4 m_vScale = {};
	_float4 m_vRotationAxis = {};
	_float	m_fRotationAngle = { 0.f };

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
