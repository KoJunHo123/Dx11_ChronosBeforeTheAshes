#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum TYPE { TYPE_WALK, TYPE_FALL, TYPE_END};
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) const {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

	_int Get_Neighbor(LINE eLine) {
		return m_iNeighborIndices[eLine];
	}

	TYPE Get_CellType() {
		return m_eType;
	}

	void Set_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_iNeighborIndices[eLine] = pNeighbor->m_iIndex;
	}

	void Set_CellType(_uint iType) {
		if (TYPE_END <= iType)
			return;
		m_eType = (TYPE)iType;
	}

	void Change_Type() {
		if (TYPE_WALK == m_eType)
			m_eType = TYPE_FALL;
		else
			m_eType = TYPE_WALK;
	}

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex, TYPE eType);
	_bool Compare_Points(_fvector vSour, _fvector vDest);
	_bool isIn(_fvector vPosition, _int* pNeighborIndex);

	_bool is_Picking(_matrix& WorldMatrix, _vector* pOut);

	void Move_Cell_17_75(_matrix& WorldMatrix, _vector* pOut);

	_vector Get_Normal();

#ifdef _DEBUG
public:
	HRESULT Render();
	HRESULT RenderSecond();
#endif

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	_int							m_iIndex = {};
	_float3							m_vPoints[POINT_END] = {};
	_int							m_iNeighborIndices[LINE_END] = { -1, -1, -1 };

	TYPE							m_eType = { TYPE_WALK };

	_bool				m_bMove = { false };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
	class CVIBuffer_Cell* m_pSecondBuffer = { nullptr };
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex, TYPE eType = TYPE_WALK);
	virtual void Free() override;
};

END