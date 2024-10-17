#include "stdafx.h"
#include "Boss_Lab_Charge_Smoke.h"
#include "GameInstance.h"

CBoss_Lab_Charge_Smoke::CBoss_Lab_Charge_Smoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CBoss_Lab_Charge_Smoke::CBoss_Lab_Charge_Smoke(const CBoss_Lab_Charge_Smoke& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CBoss_Lab_Charge_Smoke::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Lab_Charge_Smoke::Initialize(void* pArg)
{
	BOSSCHARGE_SMOKE_DESC* pDesc = static_cast<BOSSCHARGE_SMOKE_DESC*>(pArg);

	m_fSpeed = pDesc->fSpeed;
	m_fGravity = pDesc->fGravity;
	m_vScale = pDesc->vScale;
	m_vColor = pDesc->vColor;
	m_vPivot = pDesc->vPivot;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	Set_Position(XMLoadFloat3(&pDesc->vPos));

	return S_OK;
}

_uint CBoss_Lab_Charge_Smoke::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CBoss_Lab_Charge_Smoke::Update(_float fTimeDelta)
{
	if (true == m_bOn)
	{
		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

		_vector vCamPos = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW).r[3];
		vCamPos = XMVector3TransformCoord(vCamPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

		if (true == m_pVIBufferCom->Spread_Dir(XMLoadFloat3(&m_vPivot), vCamPos, m_fSpeed, m_fGravity, m_bLoop, fTimeDelta))
		{
			m_pVIBufferCom->Reset();
			m_bOn = false;
		}
		else
			m_pVIBufferCom->Scaling(XMLoadFloat3(&m_vScale), fTimeDelta);
	}
}

void CBoss_Lab_Charge_Smoke::Late_Update(_float fTimeDelta)
{
	if(true == m_bOn)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
}

HRESULT CBoss_Lab_Charge_Smoke::Render()
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

HRESULT CBoss_Lab_Charge_Smoke::Ready_Components(const BOSSCHARGE_SMOKE_DESC& Desc)
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

CBoss_Lab_Charge_Smoke* CBoss_Lab_Charge_Smoke::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Lab_Charge_Smoke* pInstance = new CBoss_Lab_Charge_Smoke(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CBoss_Lab_Charge_Smoke"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBoss_Lab_Charge_Smoke::Clone(void* pArg)
{
	CBoss_Lab_Charge_Smoke* pInstance = new CBoss_Lab_Charge_Smoke(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CBoss_Lab_Charge_Smoke"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_Lab_Charge_Smoke::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
