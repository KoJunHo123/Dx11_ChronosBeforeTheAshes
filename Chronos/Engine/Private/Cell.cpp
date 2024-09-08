#include "..\Public\Cell.h"

#ifdef _DEBUG
#include "VIBuffer_Cell.h"
#endif

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_float3 CCell::Get_CellXZCenter()
{
	_float3 vZXCenter = {};
	vZXCenter.x = m_vPoints[POINT_A].x + (m_vPoints[POINT_B].x - m_vPoints[POINT_A].x) / 2;
	vZXCenter.y = m_vPoints[POINT_A].y;
	vZXCenter.z = m_vPoints[POINT_C].z + (m_vPoints[POINT_B].z - m_vPoints[POINT_C].z) / 2;

	return vZXCenter;
}

_vector CCell::Get_CenterPos()
{
	_float fABLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_A])));
	_float fBCLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_C])));
	_float fCALength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_A])));

	_float fSum = fABLength + fBCLength + fCALength;

	_float3 vCenter = {};
	vCenter.x = (fABLength * m_vPoints[POINT_C].x + fBCLength * m_vPoints[POINT_A].x + fCALength * m_vPoints[POINT_B].x) / fSum;
	vCenter.y = (fABLength * m_vPoints[POINT_C].y + fBCLength * m_vPoints[POINT_A].y + fCALength * m_vPoints[POINT_B].y) / fSum;
	vCenter.z = (fABLength * m_vPoints[POINT_C].z + fBCLength * m_vPoints[POINT_A].z + fCALength * m_vPoints[POINT_B].z) / fSum;

	return XMLoadFloat3(&vCenter);
}

HRESULT CCell::Initialize(const _float3* pPoints, _int iIndex, TYPE eType)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	m_iIndex = iIndex;
	m_eType = eType;

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

_bool CCell::Compare_Points(_fvector vSour, _fvector vDest)
{
	if (true == XMVector3Equal(vSour, XMLoadFloat3(&m_vPoints[POINT_A])))
	{
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_B])))
			return true;
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_C])))
			return true;
	}

	if (true == XMVector3Equal(vSour, XMLoadFloat3(&m_vPoints[POINT_B])))
	{
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_C])))
			return true;
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_A])))
			return true;
	}

	if (true == XMVector3Equal(vSour, XMLoadFloat3(&m_vPoints[POINT_C])))
	{
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_A])))
			return true;
		if (true == XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_B])))
			return true;
	}

	return false;
}

_bool CCell::isIn(_fvector vPosition, _int* pNeighborIndex, _vector* pLine)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector		vSour, vDest;

		vSour = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));

		_vector		vLine = XMLoadFloat3(&m_vPoints[(i + 1) % 3]) - XMLoadFloat3(&m_vPoints[i]);
		vDest = XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f);

		if (0 < XMVectorGetX(XMVector3Dot(vSour, vDest)))
		{
			*pNeighborIndex = m_iNeighborIndices[i];
			*pLine = vLine;
			return false;
		}
	}
	return true;
}

_float CCell::Compute_Height(const _fvector& vLocalPos)
{	
	_vector vPointA = XMLoadFloat3(&m_vPoints[POINT_A]);
	_vector vPointB = XMLoadFloat3(&m_vPoints[POINT_B]);
	_vector vPointC = XMLoadFloat3(&m_vPoints[POINT_C]);

	_float4 vPlane = {};
	XMStoreFloat4(&vPlane, XMPlaneFromPoints(vPointA, vPointB, vPointC));

	_float3 vLocal = {};
	XMStoreFloat3(&vLocal, vLocalPos);

	return (-vPlane.x * vLocal.x - vPlane.z * vLocal.z - vPlane.w) / vPlane.y;
}

#ifdef _DEBUG
HRESULT CCell::Render_Walk()
{
	if(TYPE_WALK == m_eType && false == m_isActive)
	{
		m_pVIBuffer->Bind_Buffers();
		m_pVIBuffer->Render();
	}
	return S_OK;
}

HRESULT CCell::Render_Fall()
{
	if (TYPE_FALL == m_eType && false == m_isActive)
	{
		m_pVIBuffer->Bind_Buffers();
		m_pVIBuffer->Render();
	}
	return S_OK;
}
HRESULT CCell::Render_Active()
{
	if (true == m_isActive)
	{
		m_pVIBuffer->Bind_Buffers();
		m_pVIBuffer->Render();
	}
	return E_NOTIMPL;
}
#endif


CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex, TYPE eType)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex, eType)))
	{
		MSG_BOX(TEXT("Failed to Created : CCell"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCell::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
