#include "stdafx.h"
#include "Effect_Distortion.h"
#include "GameInstance.h"

CEffect_Distortion::CEffect_Distortion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect_Distortion::CEffect_Distortion(const CEffect_Distortion& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CEffect_Distortion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Distortion::Initialize(void* pArg)
{
	DISTORTION_DESC* pDesc = static_cast<DISTORTION_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_Position(XMLoadFloat3(&pDesc->vPos));
	
	m_pTransformCom->Set_Scaled(7.5f, 7.5f, 7.5f);

	return S_OK;
}

_uint CEffect_Distortion::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CEffect_Distortion::Update(_float fTimeDelta)
{
	_matrix ViewInverse = m_pGameInstance->Get_Transform_Inverse_Matrix(CPipeLine::D3DTS_VIEW);
	m_pTransformCom->LookDir(ViewInverse.r[2]);

	m_fTime += fTimeDelta;
}

void CEffect_Distortion::Late_Update(_float fTimeDelta)
{
	if (4.f < m_fTime)
		m_bDead = true;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_DISTORTION, this);
}

HRESULT CEffect_Distortion::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pNoiseTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 2)))
		return E_FAIL;
	if (FAILED(m_pGlowTexxtureCom->Bind_ShadeResource(m_pShaderCom, "g_NormTexture", 0)))
		return E_FAIL;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &m_fTime, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(10)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Distortion::Ready_Components()
{
	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Noise"),
		TEXT("Com_Texture_Noise"), reinterpret_cast<CComponent**>(&m_pNoiseTextureCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Glow"),
		TEXT("Com_Texture_Glow"), reinterpret_cast<CComponent**>(&m_pGlowTexxtureCom))))
		return E_FAIL;

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer_Rect */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Pupple"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CEffect_Distortion* CEffect_Distortion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Distortion* pInstance = new CEffect_Distortion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CEffect_Distortion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Distortion::Clone(void* pArg)
{
	CEffect_Distortion* pInstance = new CEffect_Distortion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CEffect_Distortion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Distortion::Pooling()
{
	return new CEffect_Distortion(*this);
}

void CEffect_Distortion::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pGlowTexxtureCom);
}
