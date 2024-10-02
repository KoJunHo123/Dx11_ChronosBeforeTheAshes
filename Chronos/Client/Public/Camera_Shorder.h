#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Shorder final : public CCamera
{
public:
	typedef struct : public CCamera::CAMERA_DESC
	{
		_float		fSensor;
	}CAMERA_SHORDER_DESC;
private:
	CCamera_Shorder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Shorder(const CCamera_Shorder& Prototype);
	virtual ~CCamera_Shorder() = default;

public:
	_vector Get_CameraLook() {
		return m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float				m_fSensor = { 0.f };	// 감도
	class CTransform*	m_pPlayerTransformCom = { nullptr };
	class CTransform*	m_pTargetTransformCom = { nullptr };

	_float m_fOffset = { 0.f };
	_float m_fDistance = { 0.f };
	_float m_fLimit = { 0.f };
	_float m_fSpeed = { 0.f };

	// 락온용
	_float m_fNormalLimit = { 0.f };
	_float m_fDistanceLimit = { 0.f };

	_bool m_bOnUI = { false };

public:
	static CCamera_Shorder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END