#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)
class CCamera_Interaction final : public CCamera
{
public:
	typedef struct : public CCamera::CAMERA_DESC
	{

	}CAMERA_INTERACTION_DESC;
private:
	CCamera_Interaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Interaction(const CCamera_Interaction& Prototype);
	virtual ~CCamera_Interaction() = default;

public:
	void Set_Target(_fvector vTargetPos, _float3 vCamDir) {
		XMStoreFloat3(&m_vTargetPos, vTargetPos);
		m_vCamDir = vCamDir;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float3 m_vTargetPos = {};
	_float3 m_vCamDir = {};

public:
	static CCamera_Interaction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

