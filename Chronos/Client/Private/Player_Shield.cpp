#include "stdafx.h"
#include "Player_Shield.h"
#include "GameInstance.h"

CPlayer_Shield::CPlayer_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Part(pDevice, pContext)
{
}

CPlayer_Shield::CPlayer_Shield(const CPlayer_Shield& Prototype)
	: CPlayer_Part(Prototype)
{
}

HRESULT CPlayer_Shield::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Shield::Initialize(void* pArg)
{
	PLAYER_SHIELD_DESC* pDesc = static_cast<PLAYER_SHIELD_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));

	return S_OK;
}

_uint CPlayer_Shield::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CPlayer_Shield::Update(_float fTimeDelta)
{
}

void CPlayer_Shield::Late_Update(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	// ??
	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}

	// 내 위치 * 붙여야 할 뼈의 위치 * 플레이어 위치 -> 플레이어의 위치에서 붙여야 할 뼈의 위치.
	// -> 셰이더에서 해주던 뼈 * 월드를 여기서 하는 거.
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPlayer_Shield::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
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

HRESULT CPlayer_Shield::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Shield"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CPlayer_Shield* CPlayer_Shield::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Shield* pInstance = new CPlayer_Shield(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_Shield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Shield::Clone(void* pArg)
{
	CPlayer_Shield* pInstance = new CPlayer_Shield(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CPlayer_Shield"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CPlayer_Shield::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
