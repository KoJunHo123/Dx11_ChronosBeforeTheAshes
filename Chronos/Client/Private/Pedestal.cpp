#include "stdafx.h"
#include "Pedestal.h"
#include "GameInstance.h"

#include "Pedestal_Item.h"
#include "Pedestal_InterColl.h"

CPedestal::CPedestal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject(pDevice, pContext)
{
}

CPedestal::CPedestal(const CPedestal& Prototype)
	: CContainerObject(Prototype)
{
}

HRESULT CPedestal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPedestal::Initialize(void* pArg)
{
	PEDESTAL_DESC* pDesc = static_cast<PEDESTAL_DESC*>(pArg);

	m_pItem = pDesc->pItem;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Parts(pDesc->strItemTag, pDesc->vRotation)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPos));
	m_pColliderCom->Set_OnCollision(true);

	return S_OK;
}

_uint CPedestal::Priority_Update(_float fTimeDelta)
{
	for (auto& Part : m_Parts)
	{
		if (nullptr == Part)
			continue;
		Part->Priority_Update(fTimeDelta);
	}

	return OBJ_NOEVENT;
}

void CPedestal::Update(_float fTimeDelta)
{
	for (auto& Part : m_Parts)
	{
		if (nullptr == Part)
			continue;
		Part->Update(fTimeDelta);
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CPedestal::Late_Update(_float fTimeDelta)
{
	for (auto& Part : m_Parts)
	{
		if (nullptr == Part)
			continue;
		Part->Late_Update(fTimeDelta);
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CPedestal::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ComboTexture", aiTextureType_COMBO, i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void CPedestal::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
	if (TEXT("Coll_Player") == strColliderTag)
	{
		_vector vDir = {};
		_vector vCollisionPos = pCollisionObject->Get_Position();
		vDir = vCollisionPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float fX = abs(vDir.m128_f32[0]);
		_float fY = abs(vDir.m128_f32[1]);
		_float fZ = abs(vDir.m128_f32[2]);

		_float fLength = { 0.f };

		if (fX >= fY && fX >= fZ)
		{
			vDir = XMVectorSet(vDir.m128_f32[0], 0.f, 0.f, 0.f);	// 나가야 되는 방향.
			fLength = vSourInterval.x + vDestInterval.x;	// 전체 거리.
		}
		else if (fY >= fX && fY >= fZ)
		{
			vDir = XMVectorSet(0.f, vDir.m128_f32[1], 0.f, 0.f);
			fLength = vSourInterval.y + vDestInterval.y;
		}
		else if (fZ >= fX && fZ >= fY)
		{
			vDir = XMVectorSet(0.f, 0.f, vDir.m128_f32[2], 0.f);
			fLength = vSourInterval.z + vDestInterval.z;
		}

		fLength -= XMVectorGetX(XMVector3Length(vDir));
		vDir = XMVector3Normalize(vDir) * fLength;
		// 충돌된 방향만 갖고온 extents만큼 빼주기.
		pCollisionObject->Set_Position(vCollisionPos + vDir);
	}
}

HRESULT CPedestal::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Pedestal"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider_AABB */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderAABBDesc{};
	ColliderAABBDesc.vExtents = _float3(1.f, 2.f, 1.f);
	ColliderAABBDesc.vCenter = _float3(0.f, ColliderAABBDesc.vExtents.y, 0.f);

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.pOwnerObject = this;
	ColliderDesc.pBoundingDesc = &ColliderAABBDesc;
	ColliderDesc.strColliderTag = TEXT("Coll_Interaction");

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPedestal::Ready_Parts(const _wstring strItemTag, _float3 vRotation)
{
	m_Parts.resize(PART_END);
	CPedestal_Item::PEDESTAL_ITEM_DESC ItemDesc = {};
	ItemDesc.fOffset = 4.75f;
	ItemDesc.fRotationPerSec = 0.f;
	ItemDesc.fSpeedPerSec = 1.f;
	ItemDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	ItemDesc.vRotation = vRotation;
	ItemDesc.ppItem = &m_pItem;

	if (FAILED(__super::Add_PartObject(PART_ITEM, TEXT("Prototype_GameObject_Pedestal_Item"), &ItemDesc)))
		return E_FAIL;

	CPedestal_InterColl::PART_INTERCOLL_DESC InterCollDesc = {};
	InterCollDesc.fRotationPerSec = 0.f;
	InterCollDesc.fSpeedPerSec = 0.f;
	InterCollDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	InterCollDesc.ppItem = &m_pItem;
	InterCollDesc.strItemTag = strItemTag;

	if (FAILED(__super::Add_PartObject(PART_INTERCOLL, TEXT("Prototype_GameObject_Pedestal_InterColl"), &InterCollDesc)))
		return E_FAIL;

	return S_OK;
}

CPedestal* CPedestal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPedestal* pInstance = new CPedestal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Creaet Failed : CPedestal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPedestal::Clone(void* pArg)
{
	CPedestal* pInstance = new CPedestal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CPedestal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPedestal::Pooling()
{
	return new CPedestal(*this);
}

void CPedestal::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pItem);
}
