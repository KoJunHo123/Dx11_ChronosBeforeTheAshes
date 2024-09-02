#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool isPicking(const _matrix& WorldMatrix, _vector* pOut) override;
	void Create_Cells(class CNavigation* pNavigation, _fvector vTerrainPos);

private:
	_uint			m_iNumVerticesX = {};
	_uint			m_iNumVerticesZ = {};

	_float4*		m_pVerticesPos = {};

	_float			m_fDistancePerVertex = {};

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END