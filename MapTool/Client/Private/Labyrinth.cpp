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

	Set_Rasterizer();

	return S_OK;
}

void CLabyrinth::Priority_Update(_float fTimeDelta)
{

}

void CLabyrinth::Update(_float fTimeDelta)
{
	for (auto& Collider : m_ColliderComs)
		Collider->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CLabyrinth::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);
	

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLabyrinth::Render()
{
	//m_pContext->RSSetState(m_pWireFrameRS);

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

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	//m_pContext->RSSetState(m_pSolidFrameRS);

#ifdef _DEBUG
	for(auto& Collider : m_ColliderComs)
		Collider->Render();
#endif

	m_pModelCom->Get_NumMeshes();

	return S_OK;
}

HRESULT CLabyrinth::Add_Collider(_fvector vPos, _fvector vExtents)
{
	CCollider* pColliderCom = { nullptr };

	CBounding_AABB::BOUNDING_AABB_DESC desc{};

	XMStoreFloat3(&desc.vCenter, vPos);
	XMStoreFloat3(&desc.vExtents, vExtents);
	
	_wstring strComponentTag = TEXT("Com_Collider_");
	strComponentTag += to_wstring(m_ColliderComs.size());

	/* FOR.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		strComponentTag, reinterpret_cast<CComponent**>(&pColliderCom), &desc)))
		return E_FAIL;

	m_ColliderComs.emplace_back(pColliderCom);
	return S_OK;
}

void CLabyrinth::Release_LastCollider()
{
	Safe_Release(m_ColliderComs.back());
	m_ColliderComs.erase(--m_ColliderComs.end());

}

HRESULT CLabyrinth::Set_Rasterizer()
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_NONE;
	desc.DepthClipEnable = true;

	if (FAILED(m_pDevice->CreateRasterizerState(&desc, &m_pWireFrameRS)))
		return E_FAIL;

	desc.FillMode = D3D11_FILL_SOLID;

	if (FAILED(m_pDevice->CreateRasterizerState(&desc, &m_pSolidFrameRS)))
		return E_FAIL;

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

void CLabyrinth::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	for(auto& Collider : m_ColliderComs)
	Safe_Release(Collider);

	Safe_Release(m_pWireFrameRS);
	Safe_Release(m_pSolidFrameRS);

}
