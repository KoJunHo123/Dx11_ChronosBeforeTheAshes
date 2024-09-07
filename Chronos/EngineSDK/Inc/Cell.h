#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum TYPE { TYPE_WALK, TYPE_FALL, TYPE_END };
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) const {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

	_float3 Get_NearCellIndex() {
		return _float3(m_iNeighborIndices[LINE_AB], m_iNeighborIndices[LINE_BC], m_iNeighborIndices[LINE_CA]);
	}

	TYPE Get_Type() {
		return m_eType;
	}

	_bool Get_Active() {
		return m_isActive;
	}

	void Set_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_iNeighborIndices[eLine] = pNeighbor->m_iIndex;
	}

	void Set_CellType(_uint iType) {
		if (TYPE_END <= iType)
			return;
		m_eType = (TYPE)iType;
	}

	void Set_Active(_bool isActive) {
		m_isActive = isActive;
	}

	_float3 Get_CellXZCenter();

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex, TYPE eType);
	_bool Compare_Points(_fvector vSour, _fvector vDest);
	
	_bool isIn(_fvector pPosition, _int* pNeighborIndex, _vector* pLine);

	_float Compute_Height(const _fvector& vLocalPos);


#ifdef _DEBUG
public:
	HRESULT Render_Walk();
	HRESULT Render_Fall();
	HRESULT Render_Active();
#endif

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	_int							m_iIndex = {};
	_float3							m_vPoints[POINT_END] = {};
	_int							m_iNeighborIndices[LINE_END] = { -1, -1, -1 };

	TYPE							m_eType = { TYPE_WALK };

	_bool							m_isActive = { false };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex, TYPE eType = TYPE_WALK);
	virtual void Free() override;
};

END