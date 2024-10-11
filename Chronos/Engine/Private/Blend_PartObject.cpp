#include "..\Public\Blend_PartObject.h"
#include "Shader.h"

CBlend_PartObject::CBlend_PartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject{ pDevice, pContext }
{
}

CBlend_PartObject::CBlend_PartObject(const CBlend_PartObject& Prototype)
	: CBlendObject{ Prototype }
{
}

HRESULT CBlend_PartObject::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CBlend_PartObject::Initialize(void* pArg)
{
	PARTOBJ_DESC* pDesc = static_cast<PARTOBJ_DESC*>(pArg);

	m_pParentMatrix = pDesc->pParentWorldMatrix;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

_uint CBlend_PartObject::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CBlend_PartObject::Update(_float fTimeDelta)
{
}

void CBlend_PartObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CBlend_PartObject::Render()
{
	return S_OK;
}

HRESULT CBlend_PartObject::Bind_WorldMatrix(CShader* pShader, const _char* pContantName)
{
	return pShader->Bind_Matrix(pContantName, &m_WorldMatrix);
}

void CBlend_PartObject::Free()
{
	__super::Free();

}
