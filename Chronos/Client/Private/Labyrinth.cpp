#include "stdafx.h"
#include "..\Public\Labyrinth.h"

#include "GameInstance.h"

CLabyrinth::CLabyrinth(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLabyrinth::CLabyrinth(const CLabyrinth & Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CLabyrinth::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLabyrinth::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECT_DESC		Desc{};
	
	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_vColors[COLOR_YELLOW] = _float4(1.f, 1.f, 0.4f, 1.f);
	m_vColors[COLOR_PUPPLE] = _float4(0.541f, 0.169f, 0.886f, 1.f);
	m_vColors[COLOR_RED] = _float4(0.863f, 0.078f, 0.235f, 1.f);
	m_vColors[COLOR_DEFAULT] = _float4(1.f, 1.f, 1.f, 1.f);

	return S_OK;
}

_uint CLabyrinth::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CLabyrinth::Update(_float fTimeDelta)
{

}

void CLabyrinth::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);
	

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	//m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CLabyrinth::Render()
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
		_float4 vColor = m_vColors[Select_Color(i)];
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vEmissiveColor", &vColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
		//	return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ComboTexture", aiTextureType_COMBO, i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLabyrinth::Render_LightDepth()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4		ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 100.f, -15.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CLabyrinth::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
		
	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Labyrinth"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CLabyrinth::COLOR CLabyrinth::Select_Color(_uint iMeshIndex)
{
	if (12 == iMeshIndex || 13 == iMeshIndex || 71 == iMeshIndex)
		return COLOR_RED;
	else if (35 == iMeshIndex || 37 == iMeshIndex || 72 == iMeshIndex)
		return COLOR_PUPPLE;
	else if (51 == iMeshIndex || 54 == iMeshIndex || 70 == iMeshIndex)
		return COLOR_YELLOW;

	return COLOR_DEFAULT;
}

CLabyrinth * CLabyrinth::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLabyrinth*		pInstance = new CLabyrinth(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLabyrinth"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject * CLabyrinth::Clone(void * pArg)
{
	CLabyrinth*		pInstance = new CLabyrinth(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLabyrinth"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLabyrinth::Pooling()
{
	return new CLabyrinth(*this);
}

void CLabyrinth::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	
	Safe_Release(m_pModelCom);
}
