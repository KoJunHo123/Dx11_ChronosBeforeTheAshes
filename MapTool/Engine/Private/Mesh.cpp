#include "..\Public\Mesh.h"

#include "Model.h"
#include "Shader.h"

#include "GameInstance.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer( Prototype )
{
}

HRESULT CMesh::Initialize_Prototype(const CModel* pModel, ifstream* pInfile, _fmatrix PreTransformMatrix)
{
	pInfile->read(reinterpret_cast<char*>(m_szName), sizeof(_char) * MAX_PATH);
	pInfile->read(reinterpret_cast<char*>(&m_iMaterialIndex), sizeof(_uint));
	pInfile->read(reinterpret_cast<char*>(&m_iNumVertices), sizeof(_uint));
	pInfile->read(reinterpret_cast<char*>(&m_iNumIndices), sizeof(_uint));

	m_iNumVertexBuffers = 1;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	HRESULT hr = pModel->Get_ModelType() == CModel::TYPE_NONANIM ? Ready_VertexBuffer_NonAnim(pInfile, PreTransformMatrix) : Ready_VertexBuffer_Anim(pModel, pInfile);
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

	_uint* pIndices = new _uint[m_iNumIndices];
	m_pIndices = new _uint[m_iNumIndices];

	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumIndices / 3; i++)
	{
		pInfile->read(reinterpret_cast<char*>(&pIndices[iNumIndices]), sizeof(_uint));
		m_pIndices[iNumIndices] = pIndices[iNumIndices];
		++iNumIndices;

		pInfile->read(reinterpret_cast<char*>(&pIndices[iNumIndices]), sizeof(_uint));
		m_pIndices[iNumIndices] = pIndices[iNumIndices];
		++iNumIndices;

		pInfile->read(reinterpret_cast<char*>(&pIndices[iNumIndices]), sizeof(_uint));
		m_pIndices[iNumIndices] = pIndices[iNumIndices];
		++iNumIndices;
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

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(const CModel* pModel, CShader* pShader, const _char* pConstantName)
{
	// 일단 행렬들 싹다 초기화
	ZeroMemory(m_BoneMatrices, sizeof(_float4x4) * g_iMaxMeshBones);

	// 애님 메쉬 점 찍을 때 저장해둔 뼈의 개수.
	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * pModel->Get_BoneCombindTransformationMatrix(m_BoneIndices[i]));
	}

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

_bool CMesh::isPicking(const _matrix& WorldMatrix, _vector* pOut, _float* pDist)
{
	/* 마우스 레이와 레이포스를 로컬로 던졌다. */
	m_pGameInstance->Transform_MouseRay_ToLocalSpace(WorldMatrix);

	_uint iIndex = { 0 };
	_bool isPicking = { false };

	for (size_t i = 0; i < m_iNumIndices/3 - 1; i++)
	{
		_vector pVecticesVec[3] = {
			XMLoadFloat4(&m_pVerticesPos[m_pIndices[iIndex++]]),
			XMLoadFloat4(&m_pVerticesPos[m_pIndices[iIndex++]]),
			XMLoadFloat4(&m_pVerticesPos[m_pIndices[iIndex++]])
		};

		/* 오른쪽 위 삼각형 */
		if (true == m_pGameInstance->isPicked_InLocalSpace(pVecticesVec[0],
			pVecticesVec[1],
			pVecticesVec[2],
			pOut,
			pDist))
		{
			*pOut = XMVector4Transform(*pOut, WorldMatrix);
			isPicking = true;
		}
	}
	return isPicking;
}

void CMesh::Create_Cells(CNavigation* pNavigation, _fvector vTerrainPos)
{
	_uint iIndex = { 0 };
	_bool isPicking = { false };

	for (size_t i = 0; i < m_iNumIndices / 3 - 1; i++)
	{
		_vector pVecticesVec[3] = {
			XMLoadFloat4(&m_pVerticesPos[m_pIndices[iIndex++]]),
			XMLoadFloat4(&m_pVerticesPos[m_pIndices[iIndex++]]),
			XMLoadFloat4(&m_pVerticesPos[m_pIndices[iIndex++]])
		};

		_float aY = pVecticesVec[0].m128_f32[1];
		_float bY = pVecticesVec[1].m128_f32[1];
		_float cY = pVecticesVec[2].m128_f32[1];

		if(abs(aY - bY) < 0.01f && abs(cY - bY) < 0.01f && abs(aY - cY) < 0.01f)
		{
			if(aY > 4.724f && aY < 37.f)
				pNavigation->Add_Cell(pVecticesVec[0] + vTerrainPos, pVecticesVec[1] + vTerrainPos, pVecticesVec[2] + vTerrainPos);
		}
	}
}

HRESULT CMesh::Ready_VertexBuffer_NonAnim(ifstream* pInfile, _fmatrix PreTransformMatrix)
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
	m_pVerticesPos = new _float4[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pInfile->read(reinterpret_cast<char*>(&pVertices[i]), sizeof(VTXMESH));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));
		
		_vector vPos = XMVectorSetW(XMLoadFloat3(&pVertices[i].vPosition), 1.f);
		XMStoreFloat4(m_pVerticesPos + i, vPos);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_Anim(const CModel* pModel, ifstream* pInfile)
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

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		pInfile->read(reinterpret_cast<char*>(&pVertices[i]), sizeof(VTXANIMMESH));
	}

	/* 모델 전체의 뼈가 아닌 이 메시하나에 영향을 주는 뼈의 갯수. */
	pInfile->read(reinterpret_cast<char*>(&m_iNumBones), sizeof(_uint));
	m_OffsetMatrices.resize(m_iNumBones);

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		_char szBoneName[MAX_PATH] = {};
		_uint iNumWeights = { 0 };

		pInfile->read(reinterpret_cast<char*>(&m_OffsetMatrices[i]), sizeof(_float4x4));
		pInfile->read(reinterpret_cast<char*>(szBoneName), sizeof(_char) * MAX_PATH);
		pInfile->read(reinterpret_cast<char*>(&iNumWeights), sizeof(_uint));

		m_BoneIndices.emplace_back(pModel->Get_BoneIndex(szBoneName));

		/* 이 뼈는 몇개(mNumWeights) 정점에게 영향을 주는가? */
		for (size_t j = 0; j < iNumWeights; j++)
		{
			_uint iVertexID = { 0 };
			_float fWeight = { 0.f };
			pInfile->read(reinterpret_cast<char*>(&iVertexID), sizeof(_uint));
			pInfile->read(reinterpret_cast<char*>(&fWeight), sizeof(_float));

			/* 이 뼈가 영향을 주는 j번째 정점의 인덱스(pAIBone->mWeights[j].mVertexId)*/
			if (0.f == pVertices[iVertexID].vBlendWeights.x)
			{
				pVertices[iVertexID].vBlendIndices.x = i;
				pVertices[iVertexID].vBlendWeights.x = fWeight;
			}
			else if (0.f == pVertices[iVertexID].vBlendWeights.y)
			{
				pVertices[iVertexID].vBlendIndices.y = i;
				pVertices[iVertexID].vBlendWeights.y = fWeight;
			}
			else if (0.f == pVertices[iVertexID].vBlendWeights.z)
			{
				pVertices[iVertexID].vBlendIndices.z = i;
				pVertices[iVertexID].vBlendWeights.z = fWeight;
			}
			else
			{
				pVertices[iVertexID].vBlendIndices.w = i;
				pVertices[iVertexID].vBlendWeights.w = fWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		m_BoneIndices.emplace_back(pModel->Get_BoneIndex(m_szName));

		_float4x4	OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
		m_OffsetMatrices.emplace_back(OffsetMatrix);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}


CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CModel* pModel, ifstream* pInfile, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModel, pInfile, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

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

	Safe_Delete_Array(m_pVerticesPos);
	Safe_Delete_Array(m_pIndices);
}
