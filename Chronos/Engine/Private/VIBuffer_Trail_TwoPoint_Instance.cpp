#include "..\Public\VIBuffer_Trail_TwoPoint_Instance.h"

#include "GameInstance.h"

CVIBuffer_Trail_TwoPoint_Instance::CVIBuffer_Trail_TwoPoint_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing{ pDevice, pContext }
{
}

CVIBuffer_Trail_TwoPoint_Instance::CVIBuffer_Trail_TwoPoint_Instance(const CVIBuffer_Trail_TwoPoint_Instance& Prototype)
	: CVIBuffer_Instancing{ Prototype }
	, m_TrailPoses(Prototype.m_TrailPoses)
{
}

HRESULT CVIBuffer_Trail_TwoPoint_Instance::Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	if (FAILED(__super::Initialize_Prototype(Desc)))
		return E_FAIL;

	m_iNumVertexBuffers = 2;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOS);
	m_iNumIndices = m_iNumInstance;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_iInstanceStride = sizeof(VTXTRAIL_TWOPOINT_INSTANCE);
	m_iIndexCountPerInstance = 1;

	m_TrailPoses.resize(m_iNumInstance);

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	pVertices->vPosition = _float3(0.f, 0.f, 0.f);


	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	/* 정점버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

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

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER

	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 정적버퍼로 생성한다. */
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	m_pInstanceVertices = new VTXTRAIL_TWOPOINT_INSTANCE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXTRAILINSTANCE) * m_iNumInstance);

	VTXTRAIL_TWOPOINT_INSTANCE* pInstanceVertices = static_cast<VTXTRAIL_TWOPOINT_INSTANCE*>(m_pInstanceVertices);

	auto& iter = m_TrailPoses.begin();

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pInstanceVertices[i].vCurTopPos = {};
		pInstanceVertices[i].vCurBottomPos = {};

		pInstanceVertices[i].vPreTopPos = {};
		pInstanceVertices[i].vPreBottomPos = {};
		pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance->Get_Random(Desc.vLifeTime.x, Desc.vLifeTime.y), 0.f);
		iter->vTop = {};
		iter->vBottom = {};
		++iter;
	}

	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = m_pInstanceVertices;

#pragma endregion



	return S_OK;
}

HRESULT CVIBuffer_Trail_TwoPoint_Instance::Initialize(void* pArg)
{
#pragma region INSTANCE_BUFFER

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

_bool CVIBuffer_Trail_TwoPoint_Instance::Update_Buffer(_fvector vWorldTopPos, _fvector vWorldBottomPos, _float fTimeDelta)
{
	TWOPOINT Point = {};
	XMStoreFloat3(&Point.vTop, vWorldTopPos);
	XMStoreFloat3(&Point.vBottom, vWorldBottomPos);
	m_TrailPoses.emplace_back(Point);
	
	if (m_TrailPoses.size() > m_iNumInstance)
		m_TrailPoses.pop_front();
	
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXTRAIL_TWOPOINT_INSTANCE* pVertices = static_cast<VTXTRAIL_TWOPOINT_INSTANCE*>(SubResource.pData);

	auto& iter = m_TrailPoses.begin();

	_bool m_bOver = { true };
	for (size_t i = 0; i < m_iNumInstance; ++i)
	{
		pVertices[i].vCurTopPos = iter->vTop;
		pVertices[i].vCurBottomPos = iter->vBottom;
		if(0 == i)
		{
			pVertices[i].vPreTopPos = pVertices[i].vCurTopPos;
			pVertices[i].vPreBottomPos = pVertices[i].vCurBottomPos;
		}
		else
		{
			pVertices[i].vPreTopPos = pVertices[i-1].vCurTopPos;
			pVertices[i].vPreBottomPos = pVertices[i - 1].vCurBottomPos;
		}
		++iter;
		pVertices[i].vLifeTime.y += fTimeDelta;

		if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			m_bOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
	return m_bOver;
}


CVIBuffer_Trail_TwoPoint_Instance* CVIBuffer_Trail_TwoPoint_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc)
{
	CVIBuffer_Trail_TwoPoint_Instance* pInstance = new CVIBuffer_Trail_TwoPoint_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(Desc)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Trail_TwoPoint_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CVIBuffer_Trail_TwoPoint_Instance::Clone(void* pArg)
{
	CVIBuffer_Trail_TwoPoint_Instance* pInstance = new CVIBuffer_Trail_TwoPoint_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Trail_TwoPoint_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail_TwoPoint_Instance::Free()
{
	__super::Free();
	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pSpeed);
		Safe_Delete_Array(m_pInstanceVertices);
	}

}
