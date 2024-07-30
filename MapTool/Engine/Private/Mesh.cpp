#include "..\Public\Mesh.h"
#include "GameInstance.h"

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CMesh::CMesh(const CMesh & Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eType, const aiMesh * pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	HRESULT hr = eType == CModel::TYPE_NONANIM ? Ready_VertexBuffer_NonAnim(pAIMesh, PreTransformMatrix) : Ready_VertexBuffer_Anim(pAIMesh);
	if (FAILED(hr))
		return E_FAIL;
#pragma endregion

#pragma region INDEX_BUFFER
	/* 인덱스버퍼의 내용을 채워주곡 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_uint*		pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		aiFace		AIFace = pAIMesh->mFaces[i];

		pIndices[iNumIndices++] = AIFace.mIndices[0];
		pIndices[iNumIndices++] = AIFace.mIndices[1];
		pIndices[iNumIndices++] = AIFace.mIndices[2];
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion


	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMesh::Add_Save_NonAnimData()
{
	m_pBuffers = new MODEL_BUFFER_DESC[m_iNumVertices];

	D3D11_MAPPED_SUBRESOURCE MappedResource = {};
	HRESULT hr = m_pContext->Map(m_pVB, 0, D3D11_MAP_READ, 0, &MappedResource);
	if (FAILED(hr)) 
		return E_FAIL;
	
	VTXMESH* pVertices = reinterpret_cast<VTXMESH*>(MappedResource.pData);
	
	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		// 위치
		m_pBuffers[i].pX = (_float)pVertices[i].vPosition.x;
		m_pBuffers[i].pY = (_float)pVertices[i].vPosition.y;
		m_pBuffers[i].pZ = (_float)pVertices[i].vPosition.z;

		// 법선
		m_pBuffers[i].nX = (_float)pVertices[i].vNormal.x;
		m_pBuffers[i].nY = (_float)pVertices[i].vNormal.y;
		m_pBuffers[i].nZ = (_float)pVertices[i].vNormal.z;
		
		// 텍스처 좌표
		m_pBuffers[i].texX = (_float)pVertices[i].vTexcoord.x;
		m_pBuffers[i].texY = (_float)pVertices[i].vTexcoord.y;

		// 탄젠트
		m_pBuffers[i].tanX = (_float)pVertices[i].vTangent.x;
		m_pBuffers[i].tanY = (_float)pVertices[i].vTangent.y;
		m_pBuffers[i].tanZ = (_float)pVertices[i].vTangent.z;
	}

	m_pGameInstance->Add_SaveData(m_pBuffers, m_iNumVertices * sizeof(VTXMESH));
	m_pGameInstance->Add_SaveData(&m_iMaterialIndex, sizeof(m_iMaterialIndex));
	m_pGameInstance->Add_SaveData(&m_iNumVertices, sizeof(m_iNumVertices));
	m_pGameInstance->Add_SaveData(&m_iNumIndices, sizeof(m_iNumIndices));

	return S_OK;
}


HRESULT CMesh::Ready_VertexBuffer_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}


HRESULT CMesh::Ready_VertexBuffer_Anim(const aiMesh* pAIMesh)
{
	m_iVertexStride = sizeof(VTXANIMMESH);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	// 정점 돌면서 세팅
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	// 뼈 돌면서 세팅
	m_iNumBones = pAIMesh->mNumBones;

	// 뼈를 돌면서
	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];
		// 뼈가 영향을 주는 정점들에 가중치 할당.
		for (size_t j = 0; j < pAIBone->mNumWeights; ++j)
		{
			if (0.f == pVertices[pAIBone->mWeights->mVertexId].vBlendWeights.x)
			{
				pVertices[pAIBone->mWeights->mVertexId].vBlendIndices.x = i;
				pVertices[pAIBone->mWeights->mVertexId].vBlendWeights.x = pAIBone->mWeights->mWeight;
			}
			else if (0.f == pVertices[pAIBone->mWeights->mVertexId].vBlendWeights.y)
			{
				pVertices[pAIBone->mWeights->mVertexId].vBlendIndices.y= i;
				pVertices[pAIBone->mWeights->mVertexId].vBlendWeights.y = pAIBone->mWeights->mWeight;
			}
			else if (0.f == pVertices[pAIBone->mWeights->mVertexId].vBlendWeights.z)
			{
				pVertices[pAIBone->mWeights->mVertexId].vBlendIndices.z = i;
				pVertices[pAIBone->mWeights->mVertexId].vBlendWeights.z= pAIBone->mWeights->mWeight;
			}
			else
			{
				pVertices[pAIBone->mWeights->mVertexId].vBlendIndices.w = i;
				pVertices[pAIBone->mWeights->mVertexId].vBlendWeights.w = pAIBone->mWeights->mWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}


CMesh * CMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eType, const aiMesh * pAIMesh, _fmatrix PreTransformMatrix)
{
	CMesh*		pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pAIMesh, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent * CMesh::Clone(void * pArg)
{
	CMesh*		pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

}
