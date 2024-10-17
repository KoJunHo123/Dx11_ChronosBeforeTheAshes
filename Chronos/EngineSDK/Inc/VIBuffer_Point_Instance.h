#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Spread(_fvector vPivot, _float fSpeed, _float fGravity, _bool isLoop, _float fTimeDelta) override;
	virtual _bool Move_Dir(_fvector vDir, _float fSpeed, _float fGravity, _bool isLoop, _float fTimeDelta) override;
	virtual _bool Converge(_fvector vPivot, _float fSpeed, _bool isLoop, _float fTimeDelta) override;
	virtual _bool Revolve(_fvector vPivot, _float3 vRevolveRadian, _fvector vMoveDir, _float fSpeed, _bool isLoop, _float fTimeDelta);

	_bool Spread_Random(_fvector vPivot, _float fSpeed, _float fGravity, _bool isLoop, _float fTimeInterval, _float fTimeDelta);

	virtual void Reset() override;

	_bool Trail_Points(_fmatrix WorldMatrix, _fvector vDir, _bool isLoop, _float fTimeDelta);
	_bool Trail_Spread(_fmatrix WorldMatrix, _fvector vPivot, _float fGravity, _bool isLoop, _float fTimeDelta);

private:
	_bool m_bFirst = { false };
	_float4x4 m_PreMatrix = {};

	_float m_fTime = { 0.f };

	vector<_float3> m_CurrentRandomDir;
	vector<_float3> m_NextRandomDir;

private:
	void Set_NextRandomDir();

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
END