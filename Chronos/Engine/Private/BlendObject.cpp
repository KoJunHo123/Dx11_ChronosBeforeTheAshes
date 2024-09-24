#include "..\Public\BlendObject.h"
#include "GameInstance.h"


CBlendObject::CBlendObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CBlendObject::CBlendObject(const CBlendObject & Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CBlendObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlendObject::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CBlendObject::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CBlendObject::Update(_float fTimeDelta)
{
}

void CBlendObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CBlendObject::Render()
{
	return S_OK;
}


HRESULT CBlendObject::Compute_ViewZ(const _wstring & strTransformComTag)
{
	_vector			vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_matrix		ViewMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
	m_fViewZ = XMVector3TransformCoord(vWorldPos, ViewMatrix).m128_f32[2];

	return S_OK;
}

void CBlendObject::Free()
{
	__super::Free();

}
