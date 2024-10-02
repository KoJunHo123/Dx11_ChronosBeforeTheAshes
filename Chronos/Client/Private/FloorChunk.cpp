#include "stdafx.h"
#include "FloorChunk.h"
#include "GameInstance.h"

CFloorChunk::CFloorChunk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CFloorChunk::CFloorChunk(const CFloorChunk& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CFloorChunk::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFloorChunk::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	FLOORCHUNK_DESC* pDesc = static_cast<FLOORCHUNK_DESC*>(pArg);

	if (FAILED(Ready_Components(pDesc->strModelTag)))
		return E_FAIL;

	m_vTargetPos = pDesc->vTargetPos;
	m_iCellIndex = pDesc->iCellIndex;

	_float3 vRandomPos = { m_pGameInstance->Get_Random(-4.f, 4.f), m_pGameInstance->Get_Random(-4.f, 0.f), m_pGameInstance->Get_Random(-4.f, 4.f) };

	_vector vPos = XMLoadFloat3(&m_vTargetPos) + XMLoadFloat3(&vRandomPos);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	XMStoreFloat3(&m_vStartPos, vPos);

	m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);

    return S_OK;
}

_uint CFloorChunk::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
		return OBJ_DEAD;


	return OBJ_NOEVENT;
}

void CFloorChunk::Update(_float fTimeDelta)
{
	if( false == m_bDisappear)
	{
		m_pTransformCom->MoveTo(XMLoadFloat3(&m_vTargetPos), fTimeDelta * 5.f);

		_vector vCurrentDir = XMLoadFloat3(&m_vTargetPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vStartDir = XMLoadFloat3(&m_vTargetPos) - XMLoadFloat3(&m_vStartPos);

		_float fCurrentLen = XMVectorGetX(XMVector3Length(vCurrentDir));
		_float fStartLen = XMVectorGetX(XMVector3Length(vStartDir));

		_float fRatio = { 0.f };

		if (false == m_bDisappear)
			fRatio = 1 - (fCurrentLen / fStartLen);

		m_pTransformCom->Set_Scaled(fRatio, fRatio, fRatio);
	}
	else
	{
		m_pTransformCom->MoveTo(XMLoadFloat3(&m_vStartPos), fTimeDelta * 5.f);

		_vector vCurrentDir = XMLoadFloat3(&m_vStartPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vStartDir = XMLoadFloat3(&m_vTargetPos) - XMLoadFloat3(&m_vStartPos);

		_float fCurrentLen = XMVectorGetX(XMVector3Length(vCurrentDir));
		_float fStartLen = XMVectorGetX(XMVector3Length(vStartDir));

		_float fRatio = { 0.f };

		fRatio = (fCurrentLen / fStartLen);

		m_pTransformCom->Set_Scaled(fRatio, fRatio, fRatio);
	}

	if (true == m_bDisappear && m_pTransformCom->Get_Scaled().x < 0.1f)
		m_bDead = true;
}

void CFloorChunk::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

}

HRESULT CFloorChunk::Render()
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

HRESULT CFloorChunk::Ready_Components(const _wstring strModelTag)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CFloorChunk* CFloorChunk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFloorChunk* pInstance = new CFloorChunk(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFloorChunk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFloorChunk::Clone(void* pArg)
{
	CFloorChunk* pInstance = new CFloorChunk(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFloorChunk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CFloorChunk::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
