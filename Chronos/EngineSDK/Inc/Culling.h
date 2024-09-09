#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CCulling final : public CBase
{
private:
	CCulling(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCulling() = default;

public:
	HRESULT Initialize();
	void Culling_Update(_float fDeltaTime);
	HRESULT is_Culling(class CTransform* pTransform);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	_float3			m_CulVertices[8] = {};
	_float3			m_WorldVertices[8] = {};
	_float4			m_Plane[6] = {};

public:
	static CCulling* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END