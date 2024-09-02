#include "..\Public\Bounding_AABB.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_AABB::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
	/* 기초 로컬에서의 상태. */
	BOUNDING_AABB_DESC* pDesc = static_cast<BOUNDING_AABB_DESC*>(pBoundingDesc);

	m_pOriginalBoundingDesc = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
	m_pBoundingDesc = new BoundingBox(*m_pOriginalBoundingDesc);

	return S_OK;
}

void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
	_matrix			TransformMatrix = WorldMatrix;

	// 회전 날리는거.
	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[0]);
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[1]);
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(WorldMatrix.r[2]);

	m_pOriginalBoundingDesc->Transform(*m_pBoundingDesc, TransformMatrix);
}

HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
#ifdef _DEBUG
	DX::Draw(pBatch, *m_pBoundingDesc);
#endif

	return S_OK;
}

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CBounding_AABB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundingDesc);
	Safe_Delete(m_pOriginalBoundingDesc);
}
