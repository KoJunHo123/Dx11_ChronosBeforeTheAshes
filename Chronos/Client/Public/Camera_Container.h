#pragma once

#include "ContainerObject.h"
#include "Client_Defines.h"


BEGIN(Client)
class CCamera_Container final : public CContainerObject
{
public:
	enum CAMERA { CAMERA_SHORDER, CAMERA_INTERACTION, CAMERA_END };

private:
	CCamera_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Container(const CCamera_Container& Prototype);
	virtual ~CCamera_Container() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual class CPartObject* Get_PartObject(_uint iIndex = CAMERA_END) override;
	_uint Get_CurrentCamera() {
		return m_eCurrentCamera;
	}
	void Set_CurrentCamera(_uint iCameraIndex) {
		if (CAMERA_END <= iCameraIndex)
			return;
		m_eCurrentCamera = (CAMERA)iCameraIndex;

		if (CAMERA_SHORDER == m_eCurrentCamera)
			ShowCursor(FALSE);
		else
			ShowCursor(TRUE);

	}
	void Set_InteractionTarget(_fvector vTargetPos, _float3 vCamDir);

public:


private:
	CAMERA m_eCurrentCamera = { CAMERA_END };

private:
	HRESULT Ready_Cameras();

public:
	static CCamera_Container* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};
END
