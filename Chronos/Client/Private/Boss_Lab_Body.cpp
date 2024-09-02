#include "stdafx.h"
#include "Boss_Lab_Body.h"
#include "GameInstance.h"

CBoss_Lab_Body::CBoss_Lab_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CBoss_Lab_Body::CBoss_Lab_Body(const CBoss_Lab_Body& Prototype)
	: CPartObject(Prototype)
{
}

const _float4x4* CBoss_Lab_Body::Get_BoneMatrix_Ptr(const _char* pBoneName) const
{
	return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(pBoneName);
}

HRESULT CBoss_Lab_Body::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Lab_Body::Initialize(void* pArg)
{
	BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);

	m_pBossTransformCom = pDesc->pBossTransformCom;
	Safe_AddRef(m_pBossTransformCom);
	m_pNavigationCom = pDesc->pNavigationCom;
	Safe_AddRef(m_pNavigationCom);

	m_pBossAnim = pDesc->pBossAnim;
	m_pIsFinished = pDesc->pIsFinished;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBoss_Lab_Body::Priority_Update(_float fTimeDelta)
{
}

void CBoss_Lab_Body::Update(_float fTimeDelta)
{
	m_pModelCom->SetUp_Animation(*m_pBossAnim);

	Play_Animation(fTimeDelta);
}

void CBoss_Lab_Body::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBoss_Lab_Body::Render()
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
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;
		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBoss_Lab_Body::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Boss_Lab"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

void CBoss_Lab_Body::Play_Animation(_float fTimeDelta)
{
	_vector vStateChange{};

	*m_pIsFinished = m_pModelCom->Play_Animation(fTimeDelta, vStateChange);

	vStateChange = m_pBossTransformCom->Get_Rotated_Vector(vStateChange);

	_vector vBossPos = m_pBossTransformCom->Get_State(CTransform::STATE_POSITION);
	
	_vector vMovePosition = vBossPos + vStateChange;

	_vector vLine = {};

	if (nullptr == m_pNavigationCom || true == m_pNavigationCom->isMove(vMovePosition, &vLine))
		m_pBossTransformCom-> Set_State(CTransform::STATE_POSITION, vMovePosition);
	else
	{
		vLine = XMVector3Normalize(vLine);

		_float fDot = XMVectorGetX(XMVector3Dot(vLine, vStateChange));

		if (fDot < 0.f)
		{
			vLine *= -1.f;
			fDot = XMVectorGetX(XMVector3Dot(vLine, vStateChange));
		}

		vStateChange = vLine * fDot;
		vMovePosition = vBossPos + vStateChange;

		if (true == m_pNavigationCom->isMove(vMovePosition, &vLine))
			m_pBossTransformCom->Set_State(CTransform::STATE_POSITION, vMovePosition);
	}
}


CBoss_Lab_Body* CBoss_Lab_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Lab_Body* pInstance = new CBoss_Lab_Body(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_Lab_Body"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Lab_Body::Clone(void* pArg)
{
	CBoss_Lab_Body* pInstance = new CBoss_Lab_Body(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoss_Lab_Body"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Lab_Body::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pBossTransformCom);
	Safe_Release(m_pShaderCom);
}
