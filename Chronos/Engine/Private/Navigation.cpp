#include "..\Public\Navigation.h"
#include "Cell.h"

#include "Shader.h"
#include "GameInstance.h"

_float4x4 CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent{ Prototype }
	, m_Cells{ Prototype.m_Cells }

#ifdef _DEBUG
	, m_pShader{ Prototype.m_pShader }
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif
}


HRESULT CNavigation::Initialize_Prototype(const _wstring& strNavigationDataFile)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	_ulong			dwByte = {};

	HANDLE			hFile = CreateFile(strNavigationDataFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3			vPoints[3];
	CCell::TYPE		eType = { CCell::TYPE_END };

	while (true)
	{
		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		ReadFile(hFile, &eType, sizeof(CCell::TYPE), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size(), eType);
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.emplace_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

	m_iCurrentCellIndex = pDesc->iCurrentIndex;

	return S_OK;
}

void CNavigation::Update(_fmatrix TerrainWorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, TerrainWorldMatrix);
}

_bool CNavigation::isMove(_fvector vPosition, _vector* pLine)
{
	_int		iNeighborIndex = { -1 };

	_vector vLine = {};

	/* 원래 있던 삼각형 안에서 움직인거야. */
	if (true == m_Cells[m_iCurrentCellIndex]->isIn(vPosition, &iNeighborIndex, pLine) && m_iSkipTypeIndex != m_Cells[m_iCurrentCellIndex]->Get_Type())
	{
		return true;
	}
	/* 원래 있던 삼각형을 벗어난거야. */
	else
	{
		/* 나간쪽에 이웃이 있다라면. */
		if (-1 != iNeighborIndex)
		{
			// 한번에 여러칸을 나갔을 경우 현제 있는 셀까지 이웃을 계속 갱신.
			while (true)
			{
				// 한번에 나간 셀이 밖이면
				if (-1 == iNeighborIndex || m_iSkipTypeIndex == m_Cells[iNeighborIndex]->Get_Type())
				{
					return false;
				}

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex, pLine))
					break;
			}

			m_iCurrentCellIndex = iNeighborIndex;
			return true;
		}
		else
		{
			return false;
		}
	}
}

_bool CNavigation::CheckMove_ByPos(_fvector vPosition)
{
	for (auto& Cell : m_Cells)
	{
		_int iNeighbor = {};
		_vector vLine = {};
		if (true == Cell->isIn(vPosition, &iNeighbor, &vLine) && 0 == Cell->Get_Type())
			return true;	
	}
	return false;
}

_float CNavigation::Compute_Height(const _fvector& vLocalPos)
{
	return m_Cells[m_iCurrentCellIndex]->Compute_Height(vLocalPos);
}



#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float4		vColor = -1 == m_iCurrentCellIndex ? _float4(0.f, 1.f, 0.f, 1.f) : _float4(1.f, 0.f, 0.f, 1.f);
	_float4x4	WorldMatrix = m_WorldMatrix;


	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

	m_pShader->Begin(0);

	for (auto& pCell : m_Cells)
		pCell->Render_Walk();


	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	vColor = -1 == m_iCurrentCellIndex ? _float4(1.f, 0.f, 0.f, 1.f) : _float4(1.f, 0.f, 0.f, 1.f);
	WorldMatrix = m_WorldMatrix;
	WorldMatrix._42 += 0.1f;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

	m_pShader->Begin(0);

	for (auto& pCell : m_Cells)
		pCell->Render_Fall();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	vColor = -1 == m_iCurrentCellIndex ? _float4(0.f, 0.f, 0.f, 1.f) : _float4(1.f, 0.f, 0.f, 1.f);
	WorldMatrix = m_WorldMatrix;
	WorldMatrix._42 += 0.1f;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

	m_pShader->Begin(0);

	for (auto& pCell : m_Cells)
		pCell->Render_Active();


	return S_OK;
}

#endif

_float3 CNavigation::Get_NearCellIndex(_int iCellIndex)
{
	return m_Cells[iCellIndex]->Get_NearCellIndex();
}

_uint CNavigation::Get_CellType(_int iCellIndex)
{
	return m_Cells[iCellIndex]->Get_Type();
}

_bool CNavigation::Get_CellActive(_int iCellIndex)
{
	return m_Cells[iCellIndex]->Get_Active();
}

void CNavigation::Set_CellType(_int iIndex, _uint iCellState)
{
	if (m_Cells.size() <= iIndex)
		return;
	m_Cells[iIndex]->Set_CellType(iCellState);
}

void CNavigation::Set_CellActive(_int iIndex, _bool isActive)
{
	return m_Cells[iIndex]->Set_Active(isActive);
}

void CNavigation::Set_CurrentCellIndex_ByPos(_fvector vPos)
{
	_uint iIndex = 0;
	for (auto& Cell : m_Cells)
	{
		_int iNeighbor = {};
		_vector vLine = {};
		if (true == Cell->isIn(vPos, &iNeighbor, &vLine))
			m_iCurrentCellIndex = iIndex;
		++iIndex;
	}
}

_float3 CNavigation::Get_CellZXCenter(_int iIndex)
{
	return m_Cells[iIndex]->Get_CellXZCenter();
}

_vector CNavigation::Get_CellCenterPos(_int iIndex)
{
	return m_Cells[iIndex]->Get_CenterPos();
}

_vector CNavigation::Get_NearCellPos()
{
	_float3 vCellIndices = m_Cells[m_iCurrentCellIndex]->Get_NearCellIndex();

	for (size_t i = 0; i < 3; ++i)
	{
		_int iNearIndex = (_int)vCellIndices.x >> (i * 0x10);

		if (-1 == m_Cells[iNearIndex]->Get_Index()/* || m_iSkipTypeIndex == m_Cells[iNearIndex]->Get_Type()*/)
			continue;

		return m_Cells[iNearIndex]->Get_CenterPos();
	}

	return m_Cells[m_iCurrentCellIndex]->Get_CenterPos();
}

_int CNavigation::Get_CanMoveCellIndex_InNear()
{
	_float3 vCellIndices = m_Cells[m_iCurrentCellIndex]->Get_NearCellIndex();

	for (size_t i = 0; i < 3; ++i)
	{
		_int iNearIndex = (_int)vCellIndices.x >> (i * 0x10);

		if (-1 == m_Cells[iNearIndex]->Get_Index()/* || m_iSkipTypeIndex == m_Cells[iNearIndex]->Get_Type()*/)
			continue;

		return iNearIndex;
	}

	return -1;
}


HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}

	return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strNavigationDataFile)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strNavigationDataFile)))
	{
		MSG_BOX(TEXT("Failed to Created : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif
}
