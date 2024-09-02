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
	, m_strSavePath { Prototype.m_strSavePath }

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

void CNavigation::Set_CellType(_uint iIndex, _uint iCellState)
{
	if (m_Cells.size() <= iIndex)
		return;
	m_Cells[iIndex]->Set_CellType(iCellState);
}

HRESULT CNavigation::Initialize_Prototype(const _wstring& strNavigationDataFile)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_strSavePath = strNavigationDataFile;

	//if (FAILED(Load_Data()))
	//	return E_FAIL;

	//if (FAILED(SetUp_Neighbors()))
	//	return E_FAIL;

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

_bool CNavigation::isMove(_fvector vPosition)
{
	_int			iNeighborIndex = { -1 };

	/* 원래 있던 삼각형 안에서 움직인거야. */
	if (true == m_Cells[m_iCurrentCellIndex]->isIn(vPosition, &iNeighborIndex))
	{
		return true;
	}

	/* 원래 있던 삼각형을 벗어난거야. */
	else
	{
		/* 나간쪽에 이웃이 있다라면. */
		if (-1 != iNeighborIndex)
		{
			m_iCurrentCellIndex = iNeighborIndex;
			return true;
		}


		/* 나간쪽에 이웃이 없다라면. */
		else
			return false;

	}
}

HRESULT CNavigation::Add_Cell(_fvector vPointA, _fvector vPointB, _fvector vPointC)
{
	_float3 vPoints[3] = {};

	_vector vDir = vPointB - vPointA;
	_vector vNormal = { vDir.m128_f32[2] * -1.f, vDir.m128_f32[1], vDir.m128_f32[0], vDir.m128_f32[3] };
	_vector vSecondDir = vPointC - vPointA;
	_float fDot = XMVectorGetX(XMVector3Dot(vNormal, vSecondDir));
	if (fDot > 0)
	{
		XMStoreFloat3(&vPoints[0], vPointA);
		XMStoreFloat3(&vPoints[1], vPointC);
		XMStoreFloat3(&vPoints[2], vPointB);
	}
	else if (fDot < 0)
	{
		XMStoreFloat3(&vPoints[0], vPointA);
		XMStoreFloat3(&vPoints[1], vPointB);
		XMStoreFloat3(&vPoints[2], vPointC);
	}
	else
		return E_FAIL;

	CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
	if (nullptr == pCell)
		return E_FAIL;

	m_Cells.emplace_back(pCell);

	return S_OK;
}

HRESULT CNavigation::Add_WallCell(_fvector vPointA, _fvector vPointB, _fvector vPointC)
{
	_float3 vPoints[3] = {};
	XMStoreFloat3(&vPoints[0], vPointA);
	XMStoreFloat3(&vPoints[1], vPointB);
	XMStoreFloat3(&vPoints[2], vPointC);

	CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
	if (nullptr == pCell)
		return E_FAIL;

	m_WallCells.emplace_back(pCell);


	return E_NOTIMPL;
}

HRESULT CNavigation::Create_WallCell_FromCell()
{
	if(-1 == m_Cells[0]->Get_Neighbor(CCell::LINE_AB)
		&& -1 == m_Cells[0]->Get_Neighbor(CCell::LINE_BC)
		&& -1 == m_Cells[0]->Get_Neighbor(CCell::LINE_CA))
	{
		if (FAILED(SetUp_Neighbors()))
			return E_FAIL;
	}


	_uint iIndex = 0;
	for (auto& Cell : m_Cells)
	{
		if (iIndex < 4320)
		{
			++iIndex;
			continue;
		}

		for (size_t i = 0; i < CCell::LINE_END; ++i)
		{
	
			CCell::LINE eLine = (CCell::LINE)i;
			if (-1 == Cell->Get_Neighbor(eLine))
			{
				_float3 vPoints[4] = {};

				if (CCell::LINE_AB == eLine)
				{
					XMStoreFloat3(&vPoints[0], Cell->Get_Point(CCell::POINT_A));
					XMStoreFloat3(&vPoints[1], Cell->Get_Point(CCell::POINT_B));
					vPoints[0].y += 40.f;
					vPoints[1].y = vPoints[0].y;

					XMStoreFloat3(&vPoints[2], Cell->Get_Point(CCell::POINT_B));
					XMStoreFloat3(&vPoints[3], Cell->Get_Point(CCell::POINT_A));
				}
				else if (CCell::LINE_BC == eLine)
				{
					XMStoreFloat3(&vPoints[0], Cell->Get_Point(CCell::POINT_B));
					XMStoreFloat3(&vPoints[1], Cell->Get_Point(CCell::POINT_C));
					vPoints[0].y += 40.f;
					vPoints[1].y = vPoints[0].y;

					XMStoreFloat3(&vPoints[2], Cell->Get_Point(CCell::POINT_C));
					XMStoreFloat3(&vPoints[3], Cell->Get_Point(CCell::POINT_B));
				}
				else if (CCell::LINE_CA == eLine)
				{
					XMStoreFloat3(&vPoints[0], Cell->Get_Point(CCell::POINT_C));
					XMStoreFloat3(&vPoints[1], Cell->Get_Point(CCell::POINT_A));
					vPoints[0].y += 40.f;
					vPoints[1].y = vPoints[0].y;

					XMStoreFloat3(&vPoints[2], Cell->Get_Point(CCell::POINT_A));
					XMStoreFloat3(&vPoints[3], Cell->Get_Point(CCell::POINT_C));
				}

				_float3 vFinalPoints[3] = {vPoints[0], vPoints[1], vPoints[2]};

				CCell* pCell = CCell::Create(m_pDevice, m_pContext, vFinalPoints, m_Cells.size());
				if (nullptr == pCell)
					return E_FAIL;
				m_WallCells.emplace_back(pCell);

				_float3 vFinalPoints_2[3] = { vPoints[0], vPoints[2], vPoints[3] };

				CCell* pCell_2 = CCell::Create(m_pDevice, m_pContext, vFinalPoints_2, m_Cells.size());
				if (nullptr == pCell_2)
					return E_FAIL;
				m_WallCells.emplace_back(pCell_2);
			}
		}
	}

	return S_OK;
}


_bool CNavigation::Delete_PickingCell(_vector* pOut)
{
	_matrix TerrainWorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	_uint iIndex = { 0 };

	for (auto& iter = m_Cells.begin(); iter != m_Cells.end(); ++iter)
	{
		if (iIndex < 4320)
		{
			++iIndex;
			continue;
		}
		if (true == (*iter)->is_Picking(TerrainWorldMatrix, pOut))
		{
			Safe_Release(*iter);
			m_Cells.erase(iter);
			return true;
		}
	}

	return false;
}

_bool CNavigation::Delete_PickingWallCell()
{
	_matrix TerrainWorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	_uint iIndex = { 0 };
	_vector vOut = {};
	for (auto& iter = m_WallCells.begin(); iter != m_WallCells.end(); ++iter)
	{
		if (true == (*iter)->is_Picking(TerrainWorldMatrix, &vOut))
		{
			Safe_Release(*iter);
			m_WallCells.erase(iter);
			return true;
		}
	}
	return false;
}


_int CNavigation::Find_Index_PickingCell(_vector* pOut)
{
	_matrix TerrainWorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	_uint iIndex = { 0 };
	for (auto& iter = m_Cells.begin(); iter != m_Cells.end(); ++iter)
	{
		if (true == (*iter)->is_Picking(TerrainWorldMatrix, pOut))
		{
			return iIndex;
		}
		++iIndex;
	}
	return -1;
}

_bool CNavigation::Change_CellType(_vector* pOut)
{
	_matrix TerrainWorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	for (auto& Cell : m_Cells)
	{ 
		if (true == Cell->is_Picking(TerrainWorldMatrix, pOut))
		{
			Cell->Change_Type();
			return true;
		}
	}
	return false;
}

_vector CNavigation::Check_Point(_fvector vPoint)
{
	_vector vNearestPoint = vPoint;
	for (auto& Cell : m_Cells)
	{
		for(size_t i = 0; i < CCell::POINT_END; ++i)
		{
			_float fLength = XMVectorGetX(XMVector3Length(vPoint - Cell->Get_Point((CCell::POINT)i)));
			if (fLength < 1)
				vNearestPoint = Cell->Get_Point((CCell::POINT)i);
		}
	}

	for (auto& Cell : m_WallCells)
	{
		for (size_t i = 0; i < CCell::POINT_END; ++i)
		{
			_float fLength = XMVectorGetX(XMVector3Length(vPoint - Cell->Get_Point((CCell::POINT)i)));
			if (fLength < 1)
				vNearestPoint = Cell->Get_Point((CCell::POINT)i);
		}
	}

	return vNearestPoint;
}

void CNavigation::Clear_Cell()
{
	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

	for (auto& pCell : m_WallCells)
		Safe_Release(pCell);

	m_WallCells.clear();
}

void CNavigation::Delete_Cell()
{
	if (true == m_Cells.empty())
		return;
	Safe_Release(m_Cells.back());
	m_Cells.erase(--m_Cells.end());
}

void CNavigation::Move_Cell_17_75(_vector* pOut)
{
	_matrix TerrainWorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	for (auto& Cell : m_Cells)
	{
		Cell->Move_Cell_17_75(TerrainWorldMatrix, pOut);
	}
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float4 vColor = _float4(0.f, 1.f, 0.f, 1.f);
	m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));
	m_pShader->Begin(0);

	for (auto& pCell : m_Cells)
	{
		pCell->Render();
	}

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	vColor = _float4(1.f, 0.f, 0.f, 1.f);
	m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));
	m_pShader->Begin(0);

	for (auto& pCell : m_WallCells)
	{
		pCell->Render();
	}

	return S_OK;
}

#endif

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

HRESULT CNavigation::Save_Data()
{
	_ulong			dwByte = {};
	HANDLE			hFile = CreateFile(m_strSavePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	for (auto& Cell : m_Cells)
	{
		_float3 vPoints[3] = {};
		XMStoreFloat3(&vPoints[0], Cell->Get_Point(CCell::POINT_A));
		XMStoreFloat3(&vPoints[1], Cell->Get_Point(CCell::POINT_B));
		XMStoreFloat3(&vPoints[2], Cell->Get_Point(CCell::POINT_C));
		WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		CCell::TYPE eType = Cell->Get_CellType();
		WriteFile(hFile, &eType, sizeof(CCell::TYPE), &dwByte, nullptr);
	}

	CloseHandle(hFile);


	//dwByte = {};

	//_wstring strSavePath = m_strSavePath;
	//
	//strSavePath.erase(strSavePath.length() - 4, 4);
	//strSavePath += TEXT("_Wall.dat");
	//hFile = CreateFile(strSavePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	//if (0 == hFile)
	//	return E_FAIL;

	//for (auto& Cell : m_WallCells)
	//{
	//	_float3 vPoints[3] = {};
	//	XMStoreFloat3(&vPoints[0], Cell->Get_Point(CCell::POINT_A));
	//	XMStoreFloat3(&vPoints[1], Cell->Get_Point(CCell::POINT_B));
	//	XMStoreFloat3(&vPoints[2], Cell->Get_Point(CCell::POINT_C));
	//	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	//	CCell::TYPE eType = Cell->Get_CellType();
	//	WriteFile(hFile, &eType, sizeof(CCell::TYPE), &dwByte, nullptr);
	//}

	//CloseHandle(hFile);

	return S_OK;
}


HRESULT CNavigation::Load_Data()
{
	_ulong			dwByte = {};

	HANDLE			hFile = CreateFile(m_strSavePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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
	
	//dwByte = {};

	//_wstring strSavePath = m_strSavePath;

	//strSavePath.erase(strSavePath.length() - 4, 4);
	//strSavePath += TEXT("_Wall.dat");

	//			hFile = CreateFile(strSavePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//if (0 == hFile)
	//	return E_FAIL;

	//vPoints[3];
	//eType = { CCell::TYPE_END };
	//while (true)
	//{
	//	ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
	//	ReadFile(hFile, &eType, sizeof(CCell::TYPE), &dwByte, nullptr);

	//	if (0 == dwByte)
	//		break;

	//	CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size(), eType);
	//	if (nullptr == pCell)
	//		return E_FAIL;

	//	m_WallCells.emplace_back(pCell);
	//}

	//CloseHandle(hFile);
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

	for (auto& pCell : m_WallCells)
		Safe_Release(pCell);

	m_Cells.clear();


#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif
}
