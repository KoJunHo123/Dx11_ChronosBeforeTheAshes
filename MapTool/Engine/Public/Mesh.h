#pragma once

#include "VIBuffer.h"
#include "Model.h"

/* 모델 = 메시 + 메시 + 메시 .. */
/* 메시로 구분하는 이유 : 파츠의 교체를 용이하게 만들어주기 위해서.*/
/* 메시 = 정점버퍼 + 인덱스 */

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
public:
	typedef struct 
	{
		_float pX, pY, pZ;
		_float nX, nY, nZ;
		_float texX, texY;
		_float tanX, tanY, tanZ;
	}MODEL_BUFFER_DESC;

private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;	

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Add_SaveData();
	void Clear_Buffer() {
		Safe_Delete_Array(m_pBuffers);
	}
	
private:
	_uint				m_iMaterialIndex = { 0 };
	_uint				m_iNumBones = { 0 };

	MODEL_BUFFER_DESC*	m_pBuffers = { nullptr };
	CModel::TYPE m_eType = { CModel::TYPE_END };

private:
	HRESULT Ready_VertexBuffer_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_VertexBuffer_Anim(const aiMesh* pAIMesh);


public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END