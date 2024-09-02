#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent { pDevice, pContext }
{
}

_float3 CTransform::Get_Scaled() const
{
	_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	return _float3(XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_RIGHT])),
		XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_UP])),
		XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_LOOK])));	
}

_vector CTransform::Get_Rotated_Vector(_vector vExist, _bool isTurned)
{
	_vector vScale, vRotationQuat, vTranslation;
	XMMatrixDecompose(&vScale, &vRotationQuat, &vTranslation, XMLoadFloat4x4(&m_WorldMatrix));

	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(vRotationQuat);
	_vector vRotate = XMVector3TransformNormal(vExist, rotationMatrix);

	if (true == isTurned)
	{
		vRotate.m128_f32[0] *= -1.f;
		vRotate.m128_f32[2] *= -1.f;
	}

	return vRotate;
}


HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	TRANSFORM_DESC*	pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader * pShader, const _char * pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
	
}

void CTransform::Set_Scaled(_float fX, _float fY, _float fZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fZ);
}

void CTransform::LookAt(_fvector vAt)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = vAt - vPosition;

	LookDir(vLook);
}

void CTransform::LookDir(_fvector vDir)
{
	_float3		vScale = Get_Scaled();

	_vector		vLook = vDir;

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	if (true == XMVector3Equal(vLook, XMVectorSet(0.f, 0.f, 0.f, 0.f))
		|| true == XMVector3Equal(vRight, XMVectorSet(0.f, 0.f, 0.f, 0.f))
		|| true == XMVector3Equal(vUp, XMVectorSet(0.f, 0.f, 0.f, 0.f)))
		return;

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);

}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	_vector vMove = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
	_vector vMovePosition = vPosition + vMove;
	
	_vector vLine = {};

	if (nullptr == pNavigation || true == pNavigation->isMove(vMovePosition, &vLine))
		Set_State(STATE_POSITION, vMovePosition);
	else
	{
		vLine = XMVector3Normalize(vLine);

		_float fDot = XMVectorGetX(XMVector3Dot(vLine, vMove));

		if (fDot < 0.f)
		{
			vLine *= -1.f;
			fDot = XMVectorGetX(XMVector3Dot(vLine, vMove));
		}

		vMove = vLine * fDot;
		vMovePosition = vPosition + vMove;

		if(true == pNavigation->isMove(vMovePosition, &vLine))
			Set_State(STATE_POSITION, vMovePosition);
	}
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	_vector vMove = XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta * -1.f;
	_vector vMovePosition = vPosition + vMove;

	_vector vLine = {};

	if (nullptr == pNavigation || true == pNavigation->isMove(vMovePosition, &vLine))
		Set_State(STATE_POSITION, vMovePosition);
	else
	{
		vLine = XMVector3Normalize(vLine);

		_float fDot = XMVectorGetX(XMVector3Dot(vLine, vMove));

		if (fDot < 0.f)
		{
			vLine *= -1.f;
			fDot = XMVectorGetX(XMVector3Dot(vLine, vMove));
		}
		vMove = vLine * fDot;
		vMovePosition = vPosition + vMove;

		if (true == pNavigation->isMove(vMovePosition, &vLine))
			Set_State(STATE_POSITION, vMovePosition);
	}
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	_vector vMove = XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta * -1.f;
	_vector vMovePosition = vPosition + vMove;

	_vector vLine = {};

	if (nullptr == pNavigation || true == pNavigation->isMove(vMovePosition, &vLine))
		Set_State(STATE_POSITION, vMovePosition);
	else
	{
		vLine = XMVector3Normalize(vLine);

		_float fDot = XMVectorGetX(XMVector3Dot(vLine, vMove));

		if (fDot < 0.f)
		{
			vLine *= -1.f;
			fDot = XMVectorGetX(XMVector3Dot(vLine, vMove));
		}

		vMove = vLine * fDot;
		vMovePosition = vPosition + vMove;

		if (true == pNavigation->isMove(vMovePosition, &vLine))
			Set_State(STATE_POSITION, vMovePosition);
	}
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	_vector vMove = XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;
	_vector vMovePosition = vPosition + vMove;

	_vector vLine = {};

	if (nullptr == pNavigation || true == pNavigation->isMove(vMovePosition, &vLine))
		Set_State(STATE_POSITION, vMovePosition);
	else
	{
		vLine = XMVector3Normalize(vLine);

		_float fDot = XMVectorGetX(XMVector3Dot(vLine, vMove));

		if (fDot < 0.f)
		{
			vLine *= -1.f;
			fDot = XMVectorGetX(XMVector3Dot(vLine, vMove));
		}

		vMove = vLine * fDot;
		vMovePosition = vPosition + vMove;

		if (true == pNavigation->isMove(vMovePosition, &vLine))
			Set_State(STATE_POSITION, vMovePosition);
	}
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);
	
	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));	
}

void CTransform::Turn(_bool isRotationX, _bool isRotationY, _bool isRotationZ, _float fTimeDelta)
{
	// 롤 요우 피치
	// 왼손 좌표계로 x, y, z -> 이거 항공기의 기본 3축으로 하면 y -> z, z -> y임.
	_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_fRotationPerSec * fTimeDelta * isRotationX,
		m_fRotationPerSec * fTimeDelta * isRotationY,
		m_fRotationPerSec * fTimeDelta * isRotationZ);

	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_float fX, _float fY, _float fZ)
{
	_vector		vRotation = XMQuaternionRotationRollPitchYaw(fX, fY, fZ);

	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));

}

void CTransform::SetUp_OnCell(CNavigation* pNavigation)
{
	// ####주의####
	// 원래 위치 로컬로 바꿔야 되는데 셀을 옮기면 이미 큰일나는 구조라서 그냥 하는 거임. 
	_vector vPos = Get_State(STATE_POSITION);
	_float fY = pNavigation->Compute_Height(vPos);

	vPos.m128_f32[1] = fY;
	Set_State(STATE_POSITION, vPos);
}

void CTransform::Orbit(_fvector vAxis, _fvector vCenter, _float fDistance, _float fLimit, _float fTimeDelta)
{
	Set_State(STATE_POSITION, vCenter);
	
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);
	
	_vector vMovedLook = XMVector3TransformNormal(vLook, RotationMatrix);

	_vector vNonY_MovedLook = XMVectorSet(vMovedLook.m128_f32[0], 0.f, vMovedLook.m128_f32[2], vMovedLook.m128_f32[3]);

	_float fDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMovedLook), XMVector3Normalize(vNonY_MovedLook)));

	if (fDot < fLimit)
		return;

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

_bool CTransform::MoveTo(_fvector vTargetPos, _float fTimeDelta)
{
	_vector vPos = Get_State(STATE_POSITION);
	_vector vDir = vTargetPos - vPos;

	if (XMVectorGetX(XMVector3Length(vDir)) < fTimeDelta * 2.f)
	{
		Set_State(STATE_POSITION, vTargetPos);
		return true;
	}

	vDir = XMVector3Normalize(vDir);
	Set_State(STATE_POSITION, vPos + (vDir * fTimeDelta));

	return false;
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void * pArg)
{
	CTransform*		pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}


	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
