#include "Culling.h"
#include "GameInstance.h"
#include "Transform.h"

CCulling::CCulling(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCulling::Initialize()
{
	// front - near 의 투영 좌표
	m_CulVertices[0] = { -1.f, 1.f, 0.f };
	m_CulVertices[1] = { 1.f, 1.f, 0.f };
	m_CulVertices[2] = { 1.f, -1.f, 0.f };
	m_CulVertices[3] = { -1.f, -1.f, 0.f };

	// back - far 의 투영 좌표
	m_CulVertices[4] = { -1.f, 1.f, 1.f };
	m_CulVertices[5] = { 1.f, 1.f, 1.f };
	m_CulVertices[6] = { 1.f, -1.f, 1.f };
	m_CulVertices[7] = { -1.f, -1.f, 1.f };

	return S_OK;
}

void CCulling::Culling_Update(_float fDeltaTime)
{
	_matrix ViewMatrixInv{}, ProjMatrixInv{};

	::ZeroMemory(m_WorldVertices, sizeof(_float3) * 8);
	::ZeroMemory(m_Plane, sizeof(_float4) * 6);

	ViewMatrixInv = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
	ProjMatrixInv = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_PROJ);

	for (_int i = 0; i < 8; i++)
	{
		XMStoreFloat3(&m_WorldVertices[i], XMVector3TransformCoord(XMLoadFloat3(&m_CulVertices[i]), ProjMatrixInv));
		XMStoreFloat3(&m_WorldVertices[i], XMVector3TransformCoord(XMLoadFloat3(&m_WorldVertices[i]), ViewMatrixInv));
	}

	XMStoreFloat4(&m_Plane[0], XMPlaneFromPoints(XMLoadFloat3(&m_WorldVertices[1]), XMLoadFloat3(&m_WorldVertices[5]), XMLoadFloat3(&m_WorldVertices[6])));
	XMStoreFloat4(&m_Plane[1], XMPlaneFromPoints(XMLoadFloat3(&m_WorldVertices[4]), XMLoadFloat3(&m_WorldVertices[0]), XMLoadFloat3(&m_WorldVertices[3])));

	XMStoreFloat4(&m_Plane[2], XMPlaneFromPoints(XMLoadFloat3(&m_WorldVertices[4]), XMLoadFloat3(&m_WorldVertices[5]), XMLoadFloat3(&m_WorldVertices[1])));
	XMStoreFloat4(&m_Plane[3], XMPlaneFromPoints(XMLoadFloat3(&m_WorldVertices[3]), XMLoadFloat3(&m_WorldVertices[2]), XMLoadFloat3(&m_WorldVertices[6])));
	XMStoreFloat4(&m_Plane[4], XMPlaneFromPoints(XMLoadFloat3(&m_WorldVertices[5]), XMLoadFloat3(&m_WorldVertices[4]), XMLoadFloat3(&m_WorldVertices[7])));
	XMStoreFloat4(&m_Plane[5], XMPlaneFromPoints(XMLoadFloat3(&m_WorldVertices[0]), XMLoadFloat3(&m_WorldVertices[1]), XMLoadFloat3(&m_WorldVertices[2])));
}

HRESULT CCulling::is_Culling(CTransform* pTransform)
{
	_float3 vPos = {};
	XMStoreFloat3(&vPos, pTransform->Get_State(CTransform::STATE_POSITION));

	for (_int i = 0; i < 6; i++)
	{
		_float fResult = m_Plane[i].x * vPos.x + m_Plane[i].y * vPos.y + m_Plane[i].z * vPos.z + m_Plane[i].w;
		if (fResult >= 0)
			return E_FAIL; // 이러면 컬링
	}

	return S_OK;
}

CCulling* CCulling::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCulling* pInstance = new CCulling(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Create Failed : CCulling"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCulling::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
