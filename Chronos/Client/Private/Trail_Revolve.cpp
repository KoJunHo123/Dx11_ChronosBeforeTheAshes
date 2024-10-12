#include "stdafx.h"
#include "Trail_Revolve.h"
#include "GameInstance.h"

CTrail_Revolve::CTrail_Revolve(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTrail_Revolve::CTrail_Revolve(const CTrail_Revolve& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CTrail_Revolve::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrail_Revolve::Initialize(void* pArg)
{
	TRAIL_REVOLOVE_DESC* pDesc = static_cast<TRAIL_REVOLOVE_DESC*>(pArg);
	m_vColor = pDesc->vColor;
	m_VIBuffers.resize(pDesc->iTrailCount);
	m_MovePoses.resize(pDesc->iTrailCount);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	if (0.f < pDesc->fTime)
	{
		for (auto& VIBuffer : m_VIBuffers)
		{
			VIBuffer->Set_LifeTime(pDesc->fTime);
		}
	}
	

	Set_Position(XMLoadFloat3(&pDesc->vPos));
	
	for (auto& vPos : m_MovePoses)
	{
		vPos = _float3(m_pGameInstance->Get_Random(pDesc->vPos.x - pDesc->vRange.x * 0.5f, pDesc->vPos.x + pDesc->vRange.x * 0.5f),
			m_pGameInstance->Get_Random(pDesc->vPos.y - pDesc->vRange.y * 0.5f, pDesc->vPos.y + pDesc->vRange.y * 0.5f),
			m_pGameInstance->Get_Random(pDesc->vPos.z - pDesc->vRange.z * 0.5f, pDesc->vPos.z + pDesc->vRange.z * 0.5f));
	}

	m_fRotaionPerSecond = pDesc->fRotaionPerSecond;
	m_fAccel = pDesc->fAccel;
	m_fSpeed = pDesc->fSpeed;
	m_fScale = pDesc->fScale;
	m_eType = pDesc->eType;
	m_fTypeAccel = pDesc->fTypeAccel;
	return S_OK;
}

_uint CTrail_Revolve::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CTrail_Revolve::Update(_float fTimeDelta)
{
	for (auto& vPos : m_MovePoses)
	{
		_vector vDir = Get_Position() - XMLoadFloat3(&vPos);
		_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRotaionPerSecond * fTimeDelta);
		vDir = XMVector3TransformNormal(vDir, RotationMatrix);

		XMStoreFloat3(&vPos, Get_Position() - vDir * m_fDirControl);
		vPos.y += fTimeDelta * m_fSpeed;
	}

	m_fSpeed += m_fAccel * fTimeDelta;
	
	switch (m_eType)
	{
	case TYPE_SPREAD:
		m_fDirControl += fTimeDelta * m_fTypeAccel;
		break;

	case TYPE_CONVERGE:
		m_fDirControl -= fTimeDelta * m_fTypeAccel;
		break;
	}

	_uint iIndex = { 0 };

	_bool m_bOver = { false };
	for (auto& VIBuffer : m_VIBuffers)
		m_bOver = VIBuffer->Update_Buffer(XMLoadFloat3(&m_MovePoses[iIndex++]), fTimeDelta);

	if (true == m_bOver)
		m_bDead = true;
}

void CTrail_Revolve::Late_Update(_float fTimeDelta)
{
	if(FAILED(m_pGameInstance->Add_RenderObject(CRenderer::RG_BLOOM, this)))
		return;
}

HRESULT CTrail_Revolve::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof m_vColor)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &m_fScale, sizeof m_fScale)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	for(auto& VIBuffer : m_VIBuffers)
	{
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(VIBuffer->Bind_Buffers()))
			return E_FAIL;
		if (FAILED(VIBuffer->Render()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTrail_Revolve::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxTrailInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	for(size_t i = 0; i < m_VIBuffers.size(); ++i)
	{
		_wstring strVIBuffer = TEXT("Com_VIBuffer_");
		strVIBuffer += to_wstring(i);
		/* FOR.Com_VIBuffer */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Trail_Test"),
			strVIBuffer, reinterpret_cast<CComponent**>(&m_VIBuffers[i]))))
			return E_FAIL;
	}
	
	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Trail_Beam"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CTrail_Revolve* CTrail_Revolve::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrail_Revolve* pInstance = new CTrail_Revolve(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CTrail_Revolve"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrail_Revolve::Clone(void* pArg)
{
	CTrail_Revolve* pInstance = new CTrail_Revolve(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CTrail_Revolve"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_Revolve::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

	for (auto& VIBuffer : m_VIBuffers)
		Safe_Release(VIBuffer);
}
