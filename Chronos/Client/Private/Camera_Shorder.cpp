#include "stdafx.h"
#include "..\Public\Camera_Shorder.h"

#include "GameInstance.h"
#include "Monster.h"

CCamera_Shorder::CCamera_Shorder(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera { pDevice, pContext }
{
}

CCamera_Shorder::CCamera_Shorder(const CCamera_Shorder & Prototype)
	: CCamera{ Prototype }
{
}

void CCamera_Shorder::Set_InitialState()
{
	_vector vCameraPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPos + XMVectorSet(0.f, 10.f, -10.f, 0.f));
	m_pTransformCom->LookAt(vCameraPos);
}

HRESULT CCamera_Shorder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Shorder::Initialize(void * pArg)
{
	CAMERA_SHORDER_DESC*		pDesc = static_cast<CAMERA_SHORDER_DESC*>(pArg);

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

	m_fNormalLimit = 5.f;
	m_fDistanceLimit = 80.f;

	m_fSpeed = 1.f;

	return S_OK;
}

_uint CCamera_Shorder::Priority_Update(_float fTimeDelta)
{

	if (m_pGameInstance->Get_DIMouseState_Down(DIMK_WHEEL))
	{
		if (nullptr == m_pTargetMonster)
		{
			_float fShortestLength = { 0.f };
			list<CGameObject*> GameObjects = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
			for (auto& GameObject : GameObjects)
			{
				if (false == m_pGameInstance->isIn_Frustum_WorldSpace(GameObject->Get_Position()))
				{
					continue;
				}

				_vector vDir = GameObject->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				_float fDot = XMVectorGetX(XMVector3Dot(vDir, vLook));
				vLook *= fDot;

				_float fDirLength = XMVectorGetX(XMVector3Length(vDir));
				_float fLookLength = XMVectorGetX(XMVector3Length(vLook));

				_float fNormalLength = sqrt(pow(fDirLength, 2) - pow(fLookLength, 2));

				if (fLookLength < m_fDistanceLimit)
				{
					if (0.f == fShortestLength || fNormalLength < fShortestLength)
					{
						fShortestLength = fNormalLength;
						m_pTargetMonster = static_cast<CMonster*>(GameObject);
					}
				}
			}
		}
		else
		{
			m_pTargetMonster = nullptr;
		}
	}

	if(nullptr != m_pTargetMonster)
	{
		if(m_pTargetMonster->Get_HP() <= 0.f)
			m_pTargetMonster = nullptr;	
		else
		{
			_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
			vPlayerPos.m128_f32[1] += m_fOffset;

			_vector vTargetPos = m_pTargetMonster->Get_Position();
			vTargetPos.m128_f32[1] += m_fOffset * 0.5f;

			_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

			_vector vDir = vTargetPos - vPlayerPos;
			vDir = XMVector3Normalize(vDir);

			_vector vCross = XMVector3Cross(vLook, vDir);
			if (0.997f > XMVectorGetX(XMVector3Dot(vLook, vDir)))
			{
				m_pTransformCom->Orbit(vCross, vPlayerPos, m_fLimit, m_fDistance, fTimeDelta * m_fSpeed);
			}
			else
			{
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos - (vDir * m_fDistance));
			}

			m_pTransformCom->LookAt(vPlayerPos);
		}
	}
	else
	{
		_long		MouseMove = { 0 };

		_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);

		vPlayerPos.m128_f32[1] += m_fOffset;

		if(XMVectorGetY(vPlayerPos) > -5.f)
		{
			if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM_X))
			{
				m_pTransformCom->Orbit(XMVectorSet(0.f, 1.f, 0.f, 0.f), vPlayerPos, m_fLimit, m_fDistance, fTimeDelta * MouseMove * m_fSensor);
			}
			if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM_Y))
			{
				m_pTransformCom->Orbit(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), vPlayerPos, m_fLimit, m_fDistance, fTimeDelta * MouseMove * m_fSensor);
			}

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos - XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * m_fDistance);
		}

		 m_pTransformCom->LookAt(vPlayerPos);
	}

	if (0.f < m_fShakingTime)
	{
		_vector vRandom = XMVectorSet(m_pGameInstance->Get_Random_Normal(), m_pGameInstance->Get_Random_Normal(), m_pGameInstance->Get_Random_Normal(), 0.f);

		Set_Position(Get_Position() + XMVector3Normalize(vRandom) * 0.25f);
		m_fShakingTime -= fTimeDelta;
	}

	if (m_pGameInstance->Get_DIKeyState_Down(DIKEYBOARD_0))
		m_bOnUI = !m_bOnUI;

	if(false == m_bOnUI)
	{
		POINT pt = { g_iWinSizeX / 2, g_iWinSizeY / 2 };
		ClientToScreen(g_hWnd, &pt);
		SetCursorPos(pt.x, pt.y);
	}

	__super::Priority_Update(fTimeDelta);

	return OBJ_NOEVENT;
}

void CCamera_Shorder::Update(_float fTimeDelta)
{
}

void CCamera_Shorder::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Shorder::Render()
{
	return S_OK;
}

CCamera_Shorder * CCamera_Shorder::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamera_Shorder*		pInstance = new CCamera_Shorder(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCamera_Shorder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Shorder::Clone(void * pArg)
{
	CCamera_Shorder*		pInstance = new CCamera_Shorder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCamera_Shorder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Shorder::Pooling()
{
	return new CCamera_Shorder(*this);
}

void CCamera_Shorder::Free()
{
	__super::Free();
	Safe_Release(m_pPlayerTransformCom);
}
