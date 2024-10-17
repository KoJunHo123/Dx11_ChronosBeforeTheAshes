#include "stdafx.h"
#include "WayPoint.h"
#include "GameInstance.h"

#include "WayPoint_InterColl.h"
#include "WayPoint_Effect.h"

CWayPoint::CWayPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject(pDevice, pContext)
{
}

CWayPoint::CWayPoint(const CWayPoint& Prototype)
	: CContainerObject(Prototype)
{
}

HRESULT CWayPoint::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWayPoint::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	WAYPOINT_DESC* pDesc = static_cast<WAYPOINT_DESC*>(pArg);

	Set_Position(XMLoadFloat3(&pDesc->vPos));


	return S_OK;
}

_uint CWayPoint::Priority_Update(_float fTimeDelta)
{
	m_pColliderCom->Set_OnCollision(m_bIntersect);

	for (auto& Part : m_Parts)
	{
		if (nullptr == Part)
			continue;

		Part->Priority_Update(fTimeDelta);
	}

	return OBJ_NOEVENT;
}

void CWayPoint::Update(_float fTimeDelta)
{
	//if (true == m_pGameInstance->isIn_Frustum_WorldSpace(Get_Position(), 3.f))
	//{
	//	static_cast<CWayPoint_Effect*>(m_Parts[PART_EFFECT])->Set_On(true);
	//}
	//else
	//	static_cast<CWayPoint_Effect*>(m_Parts[PART_EFFECT])->Set_On(false);

	for (auto& Part : m_Parts)
	{
		if (nullptr == Part)
			continue;

		Part->Update(fTimeDelta);
	}
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CWayPoint::Late_Update(_float fTimeDelta)
{
	for (auto& Part : m_Parts)
	{
		if (nullptr == Part)
			continue;

		Part->Late_Update(fTimeDelta);
	}

	if (true == m_pGameInstance->isIn_Frustum_WorldSpace(Get_Position(), 3.f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
#ifdef _DEBUG
		m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
	}
}

HRESULT CWayPoint::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vEmissiveColor", &vColor, sizeof(_float4))))
		return E_FAIL;

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

void CWayPoint::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
	if (TEXT("Coll_Player") == strColliderTag)
	{
		_vector vDir = {};
		_vector vCollisionPos = pCollisionObject->Get_Position();
		vDir = vCollisionPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float fX = abs(XMVectorGetX(vDir));
		_float fY = abs(XMVectorGetY(vDir));
		_float fZ = abs(XMVectorGetZ(vDir));

		_float fLength = { 0.f };

		if (fX >= fY && fX >= fZ)
		{
			vDir = XMVectorSet(XMVectorGetX(vDir), 0.f, 0.f, 0.f);	// 나가야 되는 방향.
			fLength = vSourInterval.x + vDestInterval.x;	// 전체 거리.
		}
		else if (fY >= fX && fY >= fZ)
		{
			vDir = XMVectorSet(0.f, XMVectorGetY(vDir), 0.f, 0.f);
			fLength = vSourInterval.y + vDestInterval.y;
		}
		else if (fZ >= fX && fZ >= fY)
		{
			vDir = XMVectorSet(0.f, 0.f, XMVectorGetZ(vDir), 0.f);
			fLength = vSourInterval.z + vDestInterval.z;
		}

		fLength -= XMVectorGetX(XMVector3Length(vDir));
		vDir = XMVector3Normalize(vDir) * fLength;
		// 충돌된 방향만 갖고온 extents만큼 빼주기.
		pCollisionObject->Set_Position(vCollisionPos + vDir);
	}
}

HRESULT CWayPoint::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Crystal"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc = {};
	BoundingDesc.vCenter = { 0.f, m_pTransformCom->Get_Scaled().y * 0.5f, 0.f };
	BoundingDesc.vExtents = { 2.f, 5.f, 2.f };

	CCollider::COLLIDER_DESC CollDesc = {};
	CollDesc.bCollisionOnce = false;
	CollDesc.pOwnerObject = this;
	CollDesc.strColliderTag = TEXT("Coll_Obstacle");
	CollDesc.pBoundingDesc = &BoundingDesc;

	/* FOR.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWayPoint::Ready_PartObject()
{
	m_Parts.resize(PART_END);

	CWayPoint_InterColl::INTERCOLL_DESC desc = {};
	desc.fRotationPerSec = 0.f;
	desc.fSpeedPerSec = 1.f;
	desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	desc.fOffset = m_pTransformCom->Get_Scaled().y * 0.5f;
	desc.pIntersect = &m_bIntersect;

	if(FAILED(__super::Add_PartObject(PART_INTERCOLL, TEXT("Prototype_GameObject_WayPoint_InterColl"), &desc)))
		return E_FAIL;

	//CWayPoint_Effect::EFFECT_DESC EffectDesc = {};
	//EffectDesc.fRotationPerSec = 0.f;
	//EffectDesc.fSpeedPerSec = 1.f;
	//EffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

	//if (FAILED(__super::Add_PartObject(PART_EFFECT, TEXT("Prototype_GameObject_WayPoint_Effect"), &EffectDesc)))
	//	return E_FAIL;

	return S_OK;
}

CWayPoint* CWayPoint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWayPoint* pInstance = new CWayPoint(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWayPoint"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWayPoint::Clone(void* pArg)
{
	CWayPoint* pInstance = new CWayPoint(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWayPoint"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWayPoint::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}
