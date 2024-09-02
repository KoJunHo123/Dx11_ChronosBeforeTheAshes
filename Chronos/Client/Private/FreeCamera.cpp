#include "stdafx.h"
#include "..\Public\FreeCamera.h"

#include "GameInstance.h"

CFreeCamera::CFreeCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera { pDevice, pContext }
{
}

CFreeCamera::CFreeCamera(const CFreeCamera & Prototype)
	: CCamera{ Prototype }
{
}

HRESULT CFreeCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFreeCamera::Initialize(void * pArg)
{
	CAMERA_FREE_DESC*		pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);

	m_fSensor = pDesc->fSensor;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_pPlayerTransformCom = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), g_strTransformTag, 0));
	Safe_AddRef(m_pPlayerTransformCom);

	_vector vCameraPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	vCameraPos += XMVectorSet(0.f, 10.f, -10.f, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPos);

	m_fOffset = 5.f;
	m_fDistance = 10.f;
	m_fLimit = 0.3f;

	return S_OK;
}

void CFreeCamera::Priority_Update(_float fTimeDelta)
{
	_long		MouseMove = { 0 };
	
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);

	vPlayerPos.m128_f32[1] += m_fOffset;
	
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM_X))
	{
		m_pTransformCom->Orbit(XMVectorSet(0.f, 1.f, 0.f, 0.f), vPlayerPos, m_fDistance, m_fLimit, fTimeDelta * MouseMove * m_fSensor);
	}
	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM_Y))
	{
		m_pTransformCom->Orbit(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), vPlayerPos, m_fDistance, m_fLimit, fTimeDelta * MouseMove * m_fSensor);
	}
	
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos - (vLook * m_fDistance));

	m_pTransformCom->LookAt(vPlayerPos);

	POINT pt = { g_iWinSizeX / 2, g_iWinSizeY / 2 };
	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);


	__super::Priority_Update(fTimeDelta);
}

void CFreeCamera::Update(_float fTimeDelta)
{
}

void CFreeCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CFreeCamera::Render()
{
	return S_OK;
}

CFreeCamera * CFreeCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFreeCamera*		pInstance = new CFreeCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFreeCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject * CFreeCamera::Clone(void * pArg)
{
	CFreeCamera*		pInstance = new CFreeCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFreeCamera"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFreeCamera::Free()
{
	__super::Free();
	Safe_Release(m_pPlayerTransformCom);
	Safe_Release(m_pTargetTransformCom);
}
