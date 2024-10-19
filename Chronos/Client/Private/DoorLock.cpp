#include "stdafx.h"
#include "DoorLock.h"
#include "GameInstance.h"

#include "DoorLock_InterColl.h"
#include "DoorLock_Effect.h"

CDoorLock::CDoorLock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject(pDevice, pContext)
{
}

CDoorLock::CDoorLock(const CDoorLock& Prototype)
	: CContainerObject(Prototype)
{
}

HRESULT CDoorLock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDoorLock::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	if (FAILED(Ready_PartObject()))
		return E_FAIL;

	DOORLOCK_DESC* pDesc = static_cast<DOORLOCK_DESC*>(pArg);

	Set_Position(XMLoadFloat3(&pDesc->vPos));

	m_pColliderCom->Set_OnCollision(true);


	return S_OK;
}

_uint CDoorLock::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
		return OBJ_DEAD;

	for (auto& Part : m_Parts)
	{
		if (nullptr == Part)
			continue;

		Part->Priority_Update(fTimeDelta);
	}


	return OBJ_NOEVENT;
}

void CDoorLock::Update(_float fTimeDelta)
{
	for (auto& Part : m_Parts)
	{
		if (nullptr == Part)
			continue;

		Part->Update(fTimeDelta);
	}

	if(true == m_bUnLock)
	{
		m_fDelay += fTimeDelta;
		if(8.f < m_fDelay)
		{
			if(true == m_bLoop)
			{
				SOUND_DESC desc = {};
				desc.fVolume = 1.f;
				desc.fMaxDistance = DEFAULT_DISTANCE;
				XMStoreFloat3(&desc.vPos, Get_Position());

				m_pGameInstance->SoundPlay_Additional(TEXT("SFX_Labyrinth_Door_Face_Close_01.ogg"), desc);
				m_bLoop = false;
			}
			_vector vPos = Get_Position();
			if (true == m_pTransformCom->MoveTo(XMVectorSet(XMVectorGetX(vPos), 0.f, XMVectorGetZ(vPos), 1.f), fTimeDelta * 2.f))
				m_bDead = true;
		}
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CDoorLock::Late_Update(_float fTimeDelta)
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

HRESULT CDoorLock::Render()
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

void CDoorLock::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
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

		vDir = XMVectorSet(0.f, 0.f, XMVectorGetZ(vDir), 0.f);
		fLength = vSourInterval.z + vDestInterval.z;

		fLength -= XMVectorGetX(XMVector3Length(vDir));
		vDir = XMVector3Normalize(vDir) * fLength;
		// 충돌된 방향만 갖고온 extents만큼 빼주기.
		pCollisionObject->Set_Position(vCollisionPos + vDir);
	}
}

HRESULT CDoorLock::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_DoorLock"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc = {};
	BoundingDesc.vCenter = { 0.f, m_pTransformCom->Get_Scaled().y * 0.5f, 0.f };
	BoundingDesc.vExtents = { 8.f, 5.f, 1.f };

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

HRESULT CDoorLock::Ready_PartObject()
{
	m_Parts.resize(PART_END);

	CDoorLock_InterColl::INTERCOLL_DESC InterCollDesc = {};
	InterCollDesc.fRotationPerSec = 0.f;
	InterCollDesc.fSpeedPerSec = 1.f;
	InterCollDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	InterCollDesc.fOffset = m_pTransformCom->Get_Scaled().y * 0.5f;
	InterCollDesc.pUnLock = &m_bUnLock;

	if(FAILED(__super::Add_PartObject(PART_INTERCOLL, TEXT("Prototype_GameObject_DoorLock_InterColl"), &InterCollDesc)))
		return E_FAIL;

	CDoorLock_Effect::EFFECT_DESC EffectDesc = {};
	EffectDesc.fRotationPerSec = 0.f;
	EffectDesc.fSpeedPerSec = 1.f;

	EffectDesc.iNumInstance = 1000;
	EffectDesc.vCenter = { 0.f, 0.f, 0.f };
	EffectDesc.vRange = { 10.f, 1.f, 1.f };
	EffectDesc.vExceptRange = { 0.f, 0.f, 0.f };
	EffectDesc.vSize = { 0.5f, 1.f };
	EffectDesc.vSpeed = { 2.f, 4.f };
	EffectDesc.vLifeTime = { 2.f, 4.f };
	EffectDesc.vMinColor = { 0.f, 0.f, 0.f, 1.f };
	EffectDesc.vMaxColor = { 1.f, 1.f, 1.f, 1.f };
	
	EffectDesc.vScale = { 3.f, 3.f, 3.f };
	EffectDesc.fSpeed = 1.f;
	EffectDesc.fGravity = 0.f;
	EffectDesc.vColor = { 0.541f, 0.169f, 0.886f, 1.f };;
	EffectDesc.vMoveDir = { 0.f, 1.f, 0.f };

	EffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	EffectDesc.pLoop = &m_bLoop;

	if (FAILED(__super::Add_PartObject(PART_EFFECT, TEXT("Prototype_GameObject_DoorLock_Effect"), &EffectDesc)))
		return E_FAIL;


	return S_OK;
}

CDoorLock* CDoorLock::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDoorLock* pInstance = new CDoorLock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDoorLock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDoorLock::Clone(void* pArg)
{
	CDoorLock* pInstance = new CDoorLock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDoorLock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDoorLock::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}
