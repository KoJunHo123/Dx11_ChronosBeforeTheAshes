#include "stdafx.h"
#include "Puzzle_Item.h"
#include "GameInstance.h"

#include "Player.h"

CPuzzle_Item::CPuzzle_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CPuzzle_Item::CPuzzle_Item(const CPuzzle_Item& Prototype)
	: CItem(Prototype)
{
}

HRESULT CPuzzle_Item::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPuzzle_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

_uint CPuzzle_Item::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
		return OBJ_NOEVENT;

	return OBJ_NOEVENT;
}

void CPuzzle_Item::Update(_float fTimeDelta)
{

}

void CPuzzle_Item::Late_Update(_float fTimeDelta)
{

}

HRESULT CPuzzle_Item::Render(const _float4x4& WorldMatrix)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		_float4 vColor = {1.f, 1.f, 1.f, 1.f};
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vEmissiveColor", &vColor, sizeof(_float4))))
			return E_FAIL;

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

void CPuzzle_Item::Add_Item()
{
	_uint iItemCount = m_iItemCount + 1;

	if (iItemCount > m_iMaxCount)
		return;

	m_iItemCount = iItemCount;
}

_bool CPuzzle_Item::Use_Item(class CPlayer* pPlayer)
{
	if (false == __super::Use_Item())
	{
		m_bDead = true;
		return false;
	}

	return true;
}

HRESULT CPuzzle_Item::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Puzzle_ReplacementPiece"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CPuzzle_Item* CPuzzle_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPuzzle_Item* pInstance = new CPuzzle_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CPuzzle_Item"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPuzzle_Item::Clone(void* pArg)
{
	CPuzzle_Item* pInstance = new CPuzzle_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CPuzzle_Item"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPuzzle_Item::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

