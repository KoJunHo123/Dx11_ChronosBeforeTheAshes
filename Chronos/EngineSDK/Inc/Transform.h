#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct
	{
		_float		fSpeedPerSec{}, fRotationPerSec{};
	}TRANSFORM_DESC;

	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTransform() = default;

public:
	void Set_State(STATE eState, _fvector vState) {
		XMStoreFloat3((_float3*)&m_WorldMatrix.m[eState][0], vState);
	}

	void Set_WorldMatrix(_matrix WorldMatrix) {
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}

	_vector Get_State(STATE eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	_matrix Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	const _float4x4* Get_WorldMatrix_Ptr() const {
		return &m_WorldMatrix;
	}

	_float3 Get_Scaled() const;

	_matrix Get_WorldMatrix_Inverse() const {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	// 벡터를 월드 메트릭스의 회전 값만큼 돌려서 반환하는 함수.
	_vector Get_Rotated_Vector(_vector vExist, _bool isTurned = true);

public:	
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	
public:
	void Set_Scaled(_float fX, _float fY, _float fZ);

	_bool LookAt(_fvector vAt, _float fRatio = 1.f);
	_bool LookDir(_fvector vDir, _float fRatio = 1.f);

	void Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Backward(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Left(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Right(_float fTimeDelta, class CNavigation* pNavigation = nullptr);

	void Turn(_fvector vAxis, _float fTimeDelta);	
	void Turn(_bool isRotationX, _bool isRotationY, _bool isRotationZ, _float fTimeDelta);

	void Rotation(_fvector vAxis, _float fRadian);
	void Rotation(_float fX, _float fY, _float fZ);

	void SetUp_OnCell(class CNavigation* pNavigation);
	
	void Orbit(_fvector vAxis, _fvector vCenter, _float fLimit, _float fTimeDelta);

	_bool MoveTo(_fvector vTargetPos, _float fTimeDelta);

	_float Get_Distance(_fvector vTarget);

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);


private:
	_float			m_fSpeedPerSec{};
	_float			m_fRotationPerSec{};

	_float4x4		m_WorldMatrix = {};

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END