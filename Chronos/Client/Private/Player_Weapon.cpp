#include "stdafx.h"
#include "Player_Weapon.h"
#include "GameInstance.h"

CPlayer_Weapon::CPlayer_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Part(pDevice, pContext)
{
}

CPlayer_Weapon::CPlayer_Weapon(const CPlayer_Weapon& Prototype)
	: CPlayer_Part (Prototype)
{
}

HRESULT CPlayer_Weapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Weapon::Initialize(void* pArg)
{
	PLAYER_WEAPON_DESC* pDesc = static_cast<PLAYER_WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
	m_pTailSocketMatrix = pDesc->pTailSocketMatrix;

	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CPlayer_Weapon::Priority_Update(_float fTimeDelta)
{
}

void CPlayer_Weapon::Update(_float fTimeDelta)
{
	_matrix TailSocketMatrix = XMLoadFloat4x4(m_pTailSocketMatrix);
	_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	_float3 vScale = _float3(XMVectorGetX(XMVector3Length(SocketMatrix.r[0])),
		XMVectorGetX(XMVector3Length(SocketMatrix.r[1])),
		XMVectorGetX(XMVector3Length(SocketMatrix.r[2])));

	_vector vPosition = SocketMatrix.r[3];

	_vector		vLook = TailSocketMatrix.r[3] - vPosition;
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	XMStoreFloat3((_float3*)&(*m_pSocketMatrix).m[0][0], XMVector3Normalize(vRight) * vScale.x);
	XMStoreFloat3((_float3*)&(*m_pSocketMatrix).m[1][0], XMVector3Normalize(vUp) * vScale.y);
	XMStoreFloat3((_float3*)&(*m_pSocketMatrix).m[2][0], XMVector3Normalize(vLook) * vScale.z);
}

void CPlayer_Weapon::Late_Update(_float fTimeDelta)
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

HRESULT CPlayer_Weapon::Render()
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

HRESULT CPlayer_Weapon::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player_Sword"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}


CPlayer_Weapon* CPlayer_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Weapon* pInstance = new CPlayer_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_Weapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Weapon::Clone(void* pArg)
{
	CPlayer_Weapon* pInstance = new CPlayer_Weapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CPlayer_Weapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);

}
