#include "..\Public\Collider.h"


#include "GameInstance.h"
#include "GameObject.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CCollider::CCollider(const CCollider& Prototype)
	: CComponent{ Prototype }
	, m_eColliderType{ Prototype.m_eColliderType }
#ifdef _DEBUG
	, m_pBatch{ Prototype.m_pBatch }
	, m_pEffect{ Prototype.m_pEffect }
	, m_pInputLayout{ Prototype.m_pInputLayout }
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout); 
#endif
}

HRESULT CCollider::Initialize_Prototype(TYPE eColliderType)
{
	m_eColliderType = eColliderType;


#ifdef _DEBUG

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t			iShaderByteCodeLength = {};

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(pArg);
	m_pOwnerObject = pDesc->pOwnerObject;
	m_bCollisionOnce = pDesc->bCollisionOnce;
	m_strColliderTag = pDesc->strColliderTag;

	CBounding::BOUNDING_DESC* pBoundingDesc = static_cast<CBounding::BOUNDING_DESC*>(pDesc->pBoundingDesc);

	switch (m_eColliderType)
	{
	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pBoundingDesc);
		break;
	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pBoundingDesc);
		break;
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pBoundingDesc);
		break;
	}

	m_pGameInstance->Add_Collider_OnLayers(m_strColliderTag, this);

	return S_OK;
}

void CCollider::Update(const _float4x4* pWorldMatrix)
{
	m_pBounding->Update(XMLoadFloat4x4(pWorldMatrix));
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	m_pBounding->Render(m_pBatch);

	m_pBatch->End();

	return S_OK;
}
#endif

_bool CCollider::Intersect(const _wstring strColliderTag, CCollider* pTargetCollider)
{
	if (true == m_bOnCollision && true == pTargetCollider->m_bOnCollision &&true == m_pBounding->Intersect(pTargetCollider->m_eColliderType, pTargetCollider->m_pBounding))
	{
		if(true == m_bCollisionOnce)
		{
			auto& Collider = m_CollisionSet.find(pTargetCollider);

			if (Collider == m_CollisionSet.end())
				m_CollisionSet.emplace(pTargetCollider);
			else
				return false;
		}

		m_pOwnerObject->Intersect(strColliderTag, pTargetCollider->m_pOwnerObject, m_pBounding->Get_Interval(), pTargetCollider->m_pBounding->Get_Interval());
		return true;
	}

	auto& Collider = m_CollisionSet.find(pTargetCollider);

	if (Collider != m_CollisionSet.end())
		m_CollisionSet.erase(Collider);

	return false;
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX(TEXT("Failed to Created : CCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CShader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	if(true == m_isCloned)
		m_pGameInstance->Erase_Collider(m_strColliderTag, this);
#ifdef _DEBUG
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);
#endif
	m_CollisionSet.clear();

	Safe_Release(m_pBounding);
}
