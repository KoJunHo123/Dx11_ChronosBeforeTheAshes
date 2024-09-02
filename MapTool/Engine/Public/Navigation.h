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
	void Set_CellType(_uint iIndex, _uint iCellState);
public:
	virtual HRESULT Initialize_Prototype(const _wstring& strNavigationDataFile);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Update(_fmatrix TerrainWorldMatrix);
	_bool isMove(_fvector vPosition);


	HRESULT Add_Cell(_fvector vPointA, _fvector vPointB, _fvector vPointC);
	HRESULT Add_WallCell(_fvector vPointA, _fvector vPointB, _fvector vPointC);

	HRESULT Create_WallCell_FromCell();

	_bool Delete_PickingCell(_vector* pOut);
	_bool Delete_PickingWallCell();

	_int Find_Index_PickingCell(_vector* pOut);

	_bool Change_CellType(_vector* pOut);

	// 가장 가까운 점 찾아서 반환하는 함수.
	_vector Check_Point(_fvector vPoint);
	
	void Clear_Cell();
	void Delete_Cell();

	void Move_Cell_17_75(_vector* pOut);

public:
	HRESULT Save_Data();
	HRESULT Load_Data();

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	vector<class CCell*> m_Cells;
	vector<class CCell*> m_WallCells;

	_int				 m_iCurrentCellIndex = { -1 };
	static _float4x4	 m_WorldMatrix;
						 
	_wstring			 m_strSavePath = {};

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