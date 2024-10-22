#include "stdafx.h"
#include "DoorLock_Effect.h"
#include "GameInstance.h"

CDoorLock_Effect::CDoorLock_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CDoorLock_Effect::CDoorLock_Effect(const CDoorLock_Effect& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CDoorLock_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDoorLock_Effect::Initialize(void* pArg)
{
	EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	m_fSpeed = pDesc->fSpeed;
	m_fGravity = pDesc->fGravity;
	m_vScale = pDesc->vScale;
	m_vColor = pDesc->vColor;
	m_vMoveDir = pDesc->vMoveDir;

	m_pLoop = pDesc->pLoop;

	return S_OK;
}

_uint CDoorLock_Effect::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CDoorLock_Effect::Update(_float fTimeDelta)
{
	_vector vCamLook = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW).r[2];
	m_pVIBufferCom->Move_Dir(XMLoadFloat3(&m_vMoveDir), vCamLook, m_fSpeed, m_fGravity, *m_pLoop, fTimeDelta);
	m_pVIBufferCom->Scaling(XMLoadFloat3(&m_vScale), fTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));
}

void CDoorLock_Effect::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CDoorLock_Effect::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDoorLock_Effect::Ready_Components(const EFFECT_DESC& Desc)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle_Smoke_2x2"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Particle_Snow */
	CVIBuffer_Instancing::INSTANCE_DESC ParticleDesc = {};
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);

	ParticleDesc.iNumInstance = Desc.iNumInstance;
	ParticleDesc.vCenter = Desc.vCenter;
	ParticleDesc.vRange = Desc.vRange;
	ParticleDesc.vExceptRange = Desc.vExceptRange;
	ParticleDesc.vLifeTime = Desc.vLifeTime;
	ParticleDesc.vMaxColor = Desc.vMaxColor;
	ParticleDesc.vMinColor = Desc.vMinColor;
	ParticleDesc.vSize = Desc.vSize;
	ParticleDesc.vSpeed = Desc.vSpeed;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &ParticleDesc)))
		return E_FAIL;

	return S_OK;
}

CDoorLock_Effect* CDoorLock_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDoorLock_Effect* pInstance = new CDoorLock_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CDoorLock_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDoorLock_Effect::Clone(void* pArg)
{
	CDoorLock_Effect* pInstance = new CDoorLock_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CDoorLock_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDoorLock_Effect::Pooling()
{
	return new CDoorLock_Effect(*this);
}

void CDoorLock_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
