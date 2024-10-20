#include "stdafx.h"
#include "Camera_Container.h"
#include "GameInstance.h"

#include "Camera_Shorder.h"
#include "Camera_Interaction.h"

CCamera_Container::CCamera_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject(pDevice, pContext)
{
}

CCamera_Container::CCamera_Container(const CCamera_Container& Prototype)
    : CContainerObject(Prototype)
{
}

HRESULT CCamera_Container::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Container::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Cameras()))
		return E_FAIL;

	m_eCurrentCamera = CAMERA_SHORDER;
	ShowCursor(FALSE);

    return S_OK;
}

_uint CCamera_Container::Priority_Update(_float fTimeDelta)
{


	m_Parts[m_eCurrentCamera]->Priority_Update(fTimeDelta);

    return OBJ_NOEVENT;
}

void CCamera_Container::Update(_float fTimeDelta)
{
	m_Parts[m_eCurrentCamera]->Update(fTimeDelta);
}

void CCamera_Container::Late_Update(_float fTimeDelta)
{
	m_Parts[m_eCurrentCamera]->Late_Update(fTimeDelta);
}

HRESULT CCamera_Container::Render()
{

    return S_OK;
}

CPartObject* CCamera_Container::Get_PartObject(_uint iIndex)
{
	if(CAMERA_END == iIndex)
		return m_Parts[m_eCurrentCamera];

	return m_Parts[iIndex];
}

void CCamera_Container::Set_InteractionTarget(_fvector vTargetPos, _float3 vCamDir)
{
	static_cast<CCamera_Interaction*>(m_Parts[CAMERA_INTERACTION])->Set_Target(vTargetPos, vCamDir);
}

HRESULT CCamera_Container::Ready_Cameras()
{
	m_Parts.resize(CAMERA_END);

	CCamera_Shorder::CAMERA_SHORDER_DESC		ShorderDesc{};

	ShorderDesc.fSensor = 0.05f;
	ShorderDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	ShorderDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	ShorderDesc.fFovy = XMConvertToRadians(60.0f);
	ShorderDesc.fNear = 0.1f;
	ShorderDesc.fFar = 1000.f;
	ShorderDesc.fSpeedPerSec = 30.f;
	ShorderDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	ShorderDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	ShorderDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

	ShorderDesc.iCameraIndex = CAMERA_SHORDER;

	if(FAILED(__super::Add_PartObject(CAMERA_SHORDER, TEXT("Prototype_GameObject_Camera_Shorder"), &ShorderDesc)))
		return E_FAIL;

	CCamera_Interaction::CAMERA_INTERACTION_DESC InterDesc{};

	InterDesc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	InterDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	InterDesc.fFovy = XMConvertToRadians(60.0f);
	InterDesc.fNear = 0.1f;
	InterDesc.fFar = 1000.f;
	InterDesc.fSpeedPerSec = 30.f;
	InterDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	InterDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	InterDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	
	InterDesc.iCameraIndex = CAMERA_INTERACTION;

	if (FAILED(__super::Add_PartObject(CAMERA_INTERACTION, TEXT("Prototype_GameObject_Camera_Interaction"), &InterDesc)))
		return E_FAIL;

	return S_OK;
}

CCamera_Container* CCamera_Container::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Container* pInstance = new CCamera_Container(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Container"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CCamera_Container::Clone(void* pArg)
{
	CCamera_Container* pInstance = new CCamera_Container(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Container"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Container::Free()
{
	__super::Free();
}
