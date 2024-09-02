#include "..\Public\Cell.h"

#ifdef _DEBUG
#include "VIBuffer_Cell.h"
#include "Shader.h"
#endif


CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
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

	_float3 vPoints[POINT_END] = {};
	for (size_t i = 0; i < POINT_END; ++i)
	{
		vPoints[i] = m_vPoints[i];
		XMStoreFloat3(& vPoints[i], XMLoadFloat3(&vPoints[i]) + Get_Normal() * 0.5f);
	}

	m_pSecondBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, vPoints);
	if (nullptr == m_pSecondBuffer)
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

_bool CCell::isIn(_fvector vPosition, _int* pNeighborIndex)
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
			return false;
		}

	}

	return true;
}

_bool CCell::is_Picking(_matrix& WorldMatrix, _vector* pOut)
{
	return m_pVIBuffer->isPicking(WorldMatrix, pOut);
}

void CCell::Move_Cell_17_75(_matrix& WorldMatrix, _vector* pOut)
{
	if (true == m_pVIBuffer->isPicking(WorldMatrix, pOut))
	{
		if(false == m_bMove)
		{
			for (auto& vPoint : m_vPoints)
				vPoint.z += 17.75;
			m_bMove = true;
		}
	}
}


_vector CCell::Get_Normal()
{
	_vector vLineA = XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]);
	_vector vLineB = XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]);

	_vector vNormal = XMVector3Normalize(XMVector3Cross(vLineA, vLineB));

	return vNormal;
}

#ifdef _DEBUG

HRESULT CCell::Render()
{
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if(TYPE_FALL == m_eType || true == m_bMove)
	{
		m_pSecondBuffer->Bind_Buffers();
		m_pSecondBuffer->Render();
	}

	return S_OK;
}

HRESULT CCell::RenderSecond()
{
	m_pSecondBuffer->Bind_Buffers();
	m_pSecondBuffer->Render();

	return S_OK;
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
	Safe_Release(m_pSecondBuffer);
#endif

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
