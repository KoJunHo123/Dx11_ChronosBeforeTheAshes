#include "stdafx.h"
#include "RuneKey.h"
#include "GameInstance.h"

CRuneKey::CRuneKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CRuneKey::CRuneKey(const CRuneKey& Prototype)
	: CItem(Prototype)
{
}

HRESULT CRuneKey::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRuneKey::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

_uint CRuneKey::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CRuneKey::Update(_float fTimeDelta)
{
}

void CRuneKey::Late_Update(_float fTimeDelta)
{
}

HRESULT CRuneKey::Render(const _float4x4& WorldMatrix)
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
		_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
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


_bool CRuneKey::Use_Item(CPlayer* pPlayer)
{	
	return true;
}

HRESULT CRuneKey::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Item_RuneKey"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CRuneKey* CRuneKey::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRuneKey* pInstance = new CRuneKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CRuneKey"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRuneKey::Clone(void* pArg)
{
	CRuneKey* pInstance = new CRuneKey(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CRuneKey"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRuneKey::Pooling()
{
	return new CRuneKey(*this);
}

void CRuneKey::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}

