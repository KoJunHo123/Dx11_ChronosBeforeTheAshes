#include "stdafx.h"
#include "DragonHeart.h"
#include "GameInstance.h"

#include "Player.h"

CDragonHeart::CDragonHeart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CDragonHeart::CDragonHeart(const CDragonHeart& Prototype)
	: CItem(Prototype)
{
}

HRESULT CDragonHeart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDragonHeart::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iItemCount = 3;
	m_iMaxCount = m_iItemCount;

	return S_OK;
}

_uint CDragonHeart::Priority_Update(_float fTimeDelta)
{

	return OBJ_NOEVENT;
}

void CDragonHeart::Update(_float fTimeDelta)
{

}

void CDragonHeart::Late_Update(_float fTimeDelta)
{

}

HRESULT CDragonHeart::Render(const _float4x4& WorldMatrix)
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

void CDragonHeart::Add_Item()
{
	_uint iItemCount = m_iItemCount + 1;

	if (iItemCount > m_iMaxCount)
		return;

	m_iItemCount = iItemCount;
}

_bool CDragonHeart::Use_Item(class CPlayer* pPlayer)
{
	if (nullptr == pPlayer)
		return false;

	if(true == __super::Use_Item())
	{
		pPlayer->Recovery_HP();
		return true;
	}
	return false;
}

HRESULT CDragonHeart::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Item_DragonHeart"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CDragonHeart* CDragonHeart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDragonHeart* pInstance = new CDragonHeart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CDragonHeart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDragonHeart::Clone(void* pArg)
{
	CDragonHeart* pInstance = new CDragonHeart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CDragonHeart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDragonHeart::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

