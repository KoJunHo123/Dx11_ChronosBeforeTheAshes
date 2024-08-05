#pragma once

#include "VIBuffer.h"
#include "Model.h"

/* 모델 = 메시 + 메시 + 메시 .. */
/* 메시로 구분하는 이유 : 파츠의 교체를 용이하게 만들어주기 위해서.*/
/* 메시 = 정점버퍼 + 인덱스 */

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(const CModel* pModel, const aiMesh* pAIMesh, ofstream* outfile, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(const CModel* pModel, class CShader* pShader, const _char* pConstantName);

private:
	_char				m_szName[MAX_PATH] = {};
	_uint				m_iMaterialIndex = { 0 };
	_uint				m_iNumBones = { 0 };

	// 뼈의 인덱스
	vector<_uint>		m_BoneIndices;
	// 뼈의 행렬들
	_float4x4			m_BoneMatrices[g_iMaxMeshBones] = {};	// 셰이더의 뼈 개수.
	vector<_float4x4>	m_OffsetMatrices;

	

private:
	HRESULT	Ready_VertexBuffer_NonAnim(const aiMesh* pAIMesh, ofstream* outfile, _fmatrix PreTransformMatrix);
	HRESULT	Ready_VertexBuffer_Anim(const CModel* pModel, ofstream* outfile, const aiMesh* pAIMesh);


public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CModel* pModel, const aiMesh* pAIMesh, ofstream* outfile, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END