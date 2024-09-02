#include "stdafx.h"
#include "PuzzleBase.h"
#include "GameInstance.h"

CPuzzleBase::CPuzzleBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CPuzzleBase::CPuzzleBase(const CPuzzleBase& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CPuzzleBase::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPuzzleBase::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	PUZZLEBASE_DESC* pDesc = static_cast<PUZZLEBASE_DESC*>(pArg);

	m_Desc = *pDesc;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));


	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CPuzzleBase::Priority_Update(_float fTimeDelta)
{
}

void CPuzzleBase::Update(_float fTimeDelta)
{
}

void CPuzzleBase::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPuzzleBase::Render()
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

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPuzzleBase::Save_Data(ofstream* pOutFile)
{
	pOutFile->write(reinterpret_cast<const _char*>(&m_Desc), sizeof(PUZZLEBASE_DESC));

	return S_OK;
}

HRESULT CPuzzleBase::Ready_Components()
{
	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Puzzle_Base"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

CPuzzleBase* CPuzzleBase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPuzzleBase* pInstance = new CPuzzleBase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CPuzzleBase"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPuzzleBase::Clone(void* pArg)
{
	CPuzzleBase* pInstance = new CPuzzleBase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CPuzzleBase"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPuzzleBase::Free()
{
	__super::Free();
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);

}
