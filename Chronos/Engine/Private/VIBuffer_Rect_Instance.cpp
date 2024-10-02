#include "..\Public\VIBuffer_Rect_Instance.h"

#include "GameInstance.h"

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing{ pDevice, pContext }
{
}

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& Prototype)
	: CVIBuffer_Instancing{ Prototype }
{
}

HRESULT CVIBuffer_Rect_Instance::Initialize_Prototype()
{

	m_iNumVertexBuffers = 2;
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumIndices = 6;
	m_iIndexStride = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	/* 정점버퍼에 채워줄 값들을 만들기위해서 임시적으로 공간을 할당한다. */
	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

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

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	/* 인덱스버퍼를 생성한다. */
	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Initialize(void* pArg)
{
#pragma region INSTANCE_BUFFER
	INSTANCE_DESC* pDesc = static_cast<INSTANCE_DESC*>(pArg);
	if (FAILED(__super::Initialize_Prototype(*pDesc)))
		return E_FAIL;

	m_iInstanceStride = sizeof(VTXRECTINSTANCE);
	m_iIndexCountPerInstance = 6;

	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 정적버퍼로 생성한다. */
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	m_pInstanceVertices = new VTXRECTINSTANCE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXRECTINSTANCE) * m_iNumInstance);

	VTXRECTINSTANCE* pInstanceVertices = static_cast<VTXRECTINSTANCE*>(m_pInstanceVertices);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float	fScale = m_pGameInstance->Get_Random(m_vSize.x, m_vSize.y);

		pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		_float fX{}, fY{}, fZ{};

		while (true)
		{
			fX = m_pGameInstance->Get_Random(m_vCenterPos.x - m_vRange.x * 0.5f, m_vCenterPos.x + m_vRange.x * 0.5f);
			fY = m_pGameInstance->Get_Random(m_vCenterPos.y - m_vRange.y * 0.5f, m_vCenterPos.y + m_vRange.y * 0.5f);
			fZ = m_pGameInstance->Get_Random(m_vCenterPos.z - m_vRange.z * 0.5f, m_vCenterPos.z + m_vRange.z * 0.5f);
			if (abs(fX) > (m_vCenterPos.x + m_vExceptRange.x) * 0.5f || abs(fY) > (m_vCenterPos.y + m_vExceptRange.y) * 0.5f || abs(fZ) > (m_vCenterPos.z + m_vExceptRange.z) * 0.5f)
				break;
		}


		pInstanceVertices[i].vTranslation = _float4(fX, fY, fZ, 1.f);
		pInstanceVertices[i].vLifeTime = _float2(m_pGameInstance->Get_Random(m_vLifeTime.x, m_vLifeTime.y), 0.0f);
		pInstanceVertices[i].vColor =
			_float4(m_pGameInstance->Get_Random(m_vMinColor.x, m_vMaxColor.x),
				m_pGameInstance->Get_Random(m_vMinColor.y, m_vMaxColor.y),
				m_pGameInstance->Get_Random(m_vMinColor.z, m_vMaxColor.z),
				m_pGameInstance->Get_Random(m_vMinColor.w, m_vMaxColor.w));

	}

	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);
	m_InstanceInitialData.pSysMem = m_pInstanceVertices;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}
_bool CVIBuffer_Rect_Instance::Spread(_fvector vPivot, _fvector vLocalLook, _float fSpeed, _float fGravity, _bool isLoop, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	// 기존에는 SubResource에 저장된 걸 m_pVBInstance에 저장했다면,
	// m_pVBInstance에 저장된 걸 SubResource에 가져오는 거.
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXRECTINSTANCE* pVertices = static_cast<VTXRECTINSTANCE*>(SubResource.pData);
	_bool isOver = true;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - vPivot, 0.f);
		_vector vMoveDir = XMVector3Normalize(vDir) * m_pSpeed[i] * fSpeed;
		vMoveDir = XMVectorSetY(vMoveDir, XMVectorGetY(vMoveDir) - fGravity * pVertices[i].vLifeTime.y);
		pVertices[i].vLifeTime.y += fTimeDelta;

		_float3		vScale = _float3{ XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vRight))),
			XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vUp))),
			XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vLook))) };

		_vector		vLook = XMVector3Normalize(vLocalLook);
		_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
		_vector		vUp = XMVector3Cross(vLook, vRight);

		XMStoreFloat4(&pVertices[i].vRight, XMVector3Normalize(vRight) * vScale.x);
		XMStoreFloat4(&pVertices[i].vUp, XMVector3Normalize(vUp) * vScale.y);
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vLook) * vScale.z);

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * fTimeDelta);

		if (true == isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i] = static_cast<VTXRECTINSTANCE*>(m_pInstanceVertices)[i];
		}
		else if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			isOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return isOver;
}

_bool CVIBuffer_Rect_Instance::Spread_Dir(_fvector vPivot, _fvector vLocalPos, _float fSpeed, _float fGravity, _bool isLoop, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	// 기존에는 SubResource에 저장된 걸 m_pVBInstance에 저장했다면,
	// m_pVBInstance에 저장된 걸 SubResource에 가져오는 거.
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXRECTINSTANCE* pVertices = static_cast<VTXRECTINSTANCE*>(SubResource.pData);
	_bool isOver = true;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vDir = XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - vPivot, 0.f);
		_vector vMoveDir = XMVector3Normalize(vDir) * m_pSpeed[i] * fSpeed;
		vMoveDir = XMVectorSetY(vMoveDir, XMVectorGetY(vMoveDir) - fGravity * pVertices[i].vLifeTime.y);
		pVertices[i].vLifeTime.y += fTimeDelta;

		_float3		vScale = _float3{ XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vRight))),
			XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vUp))),
			XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vLook))) };

		_fvector vCamDir = vLocalPos - XMLoadFloat4(&pVertices[i].vTranslation);

		_vector		vUp = XMVector3Normalize(vDir);
		_vector     vRight = XMVector3Cross(XMVector3Normalize(vCamDir), vUp);
		_vector     vLook = XMVector3Cross(vUp, vRight);

		XMStoreFloat4(&pVertices[i].vRight, XMVector3Normalize(vRight) * vScale.x);
		XMStoreFloat4(&pVertices[i].vUp, XMVector3Normalize(vUp) * vScale.y);
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vLook) * vScale.z);

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * fTimeDelta);

		if (true == isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i]= static_cast<VTXRECTINSTANCE*>(m_pInstanceVertices)[i];
		}
		else if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			isOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return isOver;
}

_bool CVIBuffer_Rect_Instance::Converge(_fvector vPivot, _fvector vLocalLook, _float fSpeed, _bool isLoop, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	// 기존에는 SubResource에 저장된 걸 m_pVBInstance에 저장했다면,
	// m_pVBInstance에 저장된 걸 SubResource에 가져오는 거.
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXRECTINSTANCE* pVertices = static_cast<VTXRECTINSTANCE*>(SubResource.pData);
	_bool isOver = true;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vDir = XMVectorSetW(vPivot - XMLoadFloat4(&pVertices[i].vTranslation), 0.f);

		_float3		vScale = _float3{ XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vRight))),
			XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vUp))),
			XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vLook))) };

		_vector		vLook = XMVector3Normalize(vLocalLook);
		_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
		_vector		vUp = XMVector3Cross(vLook, vRight);

		XMStoreFloat4(&pVertices[i].vRight, XMVector3Normalize(vRight) * vScale.x);
		XMStoreFloat4(&pVertices[i].vUp, XMVector3Normalize(vUp) * vScale.y);
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vLook) * vScale.z);

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeed[i] * fSpeed * fTimeDelta);
		pVertices[i].vLifeTime.y += fTimeDelta;

		_float fLength = XMVectorGetX(XMVector3Length(vDir));
		_float fMoveLength = XMVectorGetX(XMVector3Length(XMVector3Normalize(vDir) * m_pSpeed[i] * fSpeed * fTimeDelta));
		if (fLength < fMoveLength)
		{
			XMStoreFloat4(&pVertices[i].vTranslation, vPivot);
			if (true == isLoop)
			{
				pVertices[i]= static_cast<VTXRECTINSTANCE*>(m_pInstanceVertices)[i];
			}
		}
		else
		{
			isOver = false;
		}

	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return isOver;
}

_bool CVIBuffer_Rect_Instance::Converge_Dir(_fvector vPivot, _fvector vLocalPos, _float fSpeed, _bool isLoop, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	// 기존에는 SubResource에 저장된 걸 m_pVBInstance에 저장했다면,
	// m_pVBInstance에 저장된 걸 SubResource에 가져오는 거.
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXRECTINSTANCE* pVertices = static_cast<VTXRECTINSTANCE*>(SubResource.pData);
	_bool isOver = true;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vDir = XMVectorSetW(vPivot - XMLoadFloat4(&pVertices[i].vTranslation), 0.f);

		_float3		vScale = _float3{ XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vRight))),
			XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vUp))),
			XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vLook))) };

		_fvector vCamDir = vLocalPos - XMLoadFloat4(&pVertices[i].vTranslation);

		_vector		vUp = XMVector3Normalize(vDir);
		_vector     vRight = XMVector3Cross(XMVector3Normalize(vCamDir), vUp);
		_vector     vLook = XMVector3Cross(vUp, vRight);


		XMStoreFloat4(&pVertices[i].vRight, XMVector3Normalize(vRight) * vScale.x);
		XMStoreFloat4(&pVertices[i].vUp, XMVector3Normalize(vUp) * vScale.y);
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vLook) * vScale.z);

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeed[i] * fSpeed * fTimeDelta);
		pVertices[i].vLifeTime.y += fTimeDelta;

		_float fLength = XMVectorGetX(XMVector3Length(vDir));
		_float fMoveLength = XMVectorGetX(XMVector3Length(XMVector3Normalize(vDir) * m_pSpeed[i] * fSpeed * fTimeDelta));
		if (fLength < fMoveLength)
		{
			XMStoreFloat4(&pVertices[i].vTranslation, vPivot);
			if (true == isLoop)
			{
				pVertices[i]= static_cast<VTXRECTINSTANCE*>(m_pInstanceVertices)[i];
			}
		}
		else
		{
			isOver = false;
		}

	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return isOver;
}

_bool CVIBuffer_Rect_Instance::Move_Dir(_fvector vDir, _fvector vLocalLook, _float fSpeed, _float fGravity, _bool isLoop, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	// 덮어쓰기 x
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXRECTINSTANCE* pVertices = static_cast<VTXRECTINSTANCE*>(SubResource.pData);
	_bool isOver = true;
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float3		vScale = _float3{ XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vRight))),
			XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vUp))),
			XMVectorGetX(XMVector3Length(XMLoadFloat4(&pVertices[i].vLook))) };

		_vector		vLook = XMVector3Normalize(vLocalLook);
		_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
		_vector		vUp = XMVector3Cross(vLook, vRight);

		XMStoreFloat4(&pVertices[i].vRight, XMVector3Normalize(vRight) * vScale.x);
		XMStoreFloat4(&pVertices[i].vUp, XMVector3Normalize(vUp) * vScale.y);
		XMStoreFloat4(&pVertices[i].vLook, XMVector3Normalize(vLook) * vScale.z);

		_vector vMoveDir = XMVector3Normalize(vDir) * m_pSpeed[i] * fSpeed;
		vMoveDir = XMVectorSetY(vMoveDir, XMVectorGetY(vMoveDir) - fGravity * pVertices[i].vLifeTime.y);
		pVertices[i].vLifeTime.y += fTimeDelta;

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * fTimeDelta);


		if (true == isLoop && pVertices[i].vLifeTime.y >= pVertices[i].vLifeTime.x)
		{
			pVertices[i] = static_cast<VTXRECTINSTANCE*>(m_pInstanceVertices)[i];
		}
		else if (pVertices[i].vLifeTime.y < pVertices[i].vLifeTime.x)
			isOver = false;
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return isOver;
}

void CVIBuffer_Rect_Instance::Scaling(_fvector vAddScale, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	// 기존에는 SubResource에 저장된 걸 m_pVBInstance에 저장했다면,
	// m_pVBInstance에 저장된 걸 SubResource에 가져오는 거.
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXRECTINSTANCE* pVertices = static_cast<VTXRECTINSTANCE*>(SubResource.pData);
	_bool isOver = true;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float3 vScaling{};
		XMStoreFloat3(&vScaling, vAddScale);

		XMStoreFloat4(&pVertices[i].vRight, XMLoadFloat4(&pVertices[i].vRight) + XMVector3Normalize(XMLoadFloat4(&pVertices[i].vRight)) * vScaling.x * fTimeDelta);
		XMStoreFloat4(&pVertices[i].vUp, XMLoadFloat4(&pVertices[i].vUp) + XMVector3Normalize(XMLoadFloat4(&pVertices[i].vUp)) * vScaling.y * fTimeDelta);
		XMStoreFloat4(&pVertices[i].vLook, XMLoadFloat4(&pVertices[i].vLook) + XMVector3Normalize(XMLoadFloat4(&pVertices[i].vLook)) * vScaling.z* fTimeDelta);
	}
	m_pContext->Unmap(m_pVBInstance, 0);

}

void CVIBuffer_Rect_Instance::Turn(_fvector vAxis, _float fRotatioPerSec, _float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	// 기존에는 SubResource에 저장된 걸 m_pVBInstance에 저장했다면,
	// m_pVBInstance에 저장된 걸 SubResource에 가져오는 거.
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXRECTINSTANCE* pVertices = static_cast<VTXRECTINSTANCE*>(SubResource.pData);
	_bool isOver = true;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector		vRight = XMLoadFloat4(&pVertices[i].vRight);
		_vector		vUp = XMLoadFloat4(&pVertices[i].vUp);
		_vector		vLook = XMLoadFloat4(&pVertices[i].vLook);

		_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRotatioPerSec * fTimeDelta);

		XMStoreFloat4(&pVertices[i].vRight, XMVector3TransformNormal(vRight, RotationMatrix));
		XMStoreFloat4(&pVertices[i].vUp, XMVector3TransformNormal(vUp, RotationMatrix));
		XMStoreFloat4(&pVertices[i].vLook, XMVector3TransformNormal(vLook, RotationMatrix));
	}
	m_pContext->Unmap(m_pVBInstance, 0);

}

void CVIBuffer_Rect_Instance::Reset()
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXRECTINSTANCE* pVertices = static_cast<VTXRECTINSTANCE*>(SubResource.pData);
	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i] = static_cast<VTXRECTINSTANCE*>(m_pInstanceVertices)[i];
	}
	m_pContext->Unmap(m_pVBInstance, 0);
}


_float CVIBuffer_Rect_Instance::Get_Radian(_fvector vMoveDir)
{
	 _vector vDir = XMVectorSet(XMVectorGetX(vMoveDir), XMVectorGetY(vMoveDir), 0.f, 0.f);
	_float fDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMVectorSet(1.f, 0.f, 0.f, 0.f)));
	_float fRadian = acos(fDot);

	if (vMoveDir.m128_f32[1] < 0.f)
		fRadian *= -1.f;

	return fRadian;
}

CVIBuffer_Rect_Instance* CVIBuffer_Rect_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Rect_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect_Instance::Clone(void* pArg)
{
	CVIBuffer_Rect_Instance* pInstance = new CVIBuffer_Rect_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Rect_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_Instance::Free()
{
	__super::Free();
	Safe_Delete_Array(m_pSpeed);
	Safe_Delete_Array(m_pInstanceVertices);
}
