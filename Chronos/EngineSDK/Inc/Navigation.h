#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct
	{
		_int			iCurrentIndex = { -1 };
	}NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& Prototype);
	virtual ~CNavigation() = default;

public:
	_uint Get_CurrentCellIndex() {
		return m_iCurrentCellIndex;
	}

	_float3 Get_NearCellIndex(_int iCellIndex);
	_uint Get_CellType(_int iCellIndex);
	_bool Get_CellActive(_int iCellIndex);
	_float3 Get_CellZXCenter(_int iIndex);

	void Set_CellType(_int iIndex, _uint iCellState);
	void Set_CellActive(_int iIndex, _bool isActive);

	void Set_SkipTypeIndex(_uint iSkipTypeIndex) {
		m_iSkipTypeIndex = iSkipTypeIndex;
	}
	

public:
	virtual HRESULT Initialize_Prototype(const _wstring& strNavigationDataFile);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Update(_fmatrix TerrainWorldMatrix);
	_bool isMove(_fvector vPosition, _vector* pLine);
	_float Compute_Height(const _fvector& vLocalPos);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	vector<class CCell*>				m_Cells;
	static _float4x4					m_WorldMatrix;
	_int								m_iCurrentCellIndex = { -1 };
	_int								m_iSkipTypeIndex = { -1 };

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

private:
	HRESULT SetUp_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strNavigationDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END