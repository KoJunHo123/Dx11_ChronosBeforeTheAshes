#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail_Instance final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Trail_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail_Instance(const CVIBuffer_Trail_Instance& Prototype);
	virtual ~CVIBuffer_Trail_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc) override;
	virtual HRESULT Initialize(void* pArg) override;

	_bool Update_Buffer(_fvector vWorldPos, _float fTimeDelta);
	void Set_LifeTime(_float fTime);

private:
	list<_float3> m_TrailPoses;
	_bool m_bFirst = { false };

public:
	static CVIBuffer_Trail_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END