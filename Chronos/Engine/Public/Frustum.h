#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();

	/* 월드스페이스 까지 여덟개 점을 이동시키고 */
	/* 평면을 구성한다. */
	void Update();

public:
	_bool isIn_WorldSpace(_fvector vPosition, _float fRadius);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	_float3					m_vPoints[8] = {};
	_float4					m_vPlanes_InWorldSpace[6];

private:
	void Make_Plane(const _float3* pPoints, _float4* pPlanes);

public:
	static CFrustum* Create();
	virtual void Free() override;
};

END