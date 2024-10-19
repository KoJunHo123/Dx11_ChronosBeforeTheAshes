#include "stdafx.h"
#include "PuzzlePart.h"
#include "GameInstance.h"

#include "PuzzleBase.h"

CPuzzlePart::CPuzzlePart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CPuzzlePart::CPuzzlePart(const CPuzzlePart& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CPuzzlePart::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CPuzzlePart::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	PUZZLEPART_DESC* pDesc = static_cast<PUZZLEPART_DESC*>(pArg);

	m_vTargetPos = pDesc->vPos;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));

	if (FAILED(Ready_Components(pDesc->strModelTag)))
		return E_FAIL;
	
	m_iCurrentLocation = pDesc->iCurrentLocation;

	for (size_t i = 0; i < 255; ++i)
	{
		m_CellIndices[i] = pDesc->pCellIndices[i];
	}

	return S_OK;
}

_uint CPuzzlePart::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CPuzzlePart::Update(_float fTimeDelta)
{
	m_pTransformCom->MoveTo(XMLoadFloat3(&m_vTargetPos), fTimeDelta * 1.25f);
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

}

void CPuzzlePart::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPuzzlePart::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
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


HRESULT CPuzzlePart::Ready_Components(const _wstring strModelTag)
{
	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

CPuzzlePart* CPuzzlePart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPuzzlePart* pInstance = new CPuzzlePart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CPuzzlePart"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPuzzlePart::Clone(void* pArg)
{
	CPuzzlePart* pInstance = new CPuzzlePart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CPuzzlePart"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPuzzlePart::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}

