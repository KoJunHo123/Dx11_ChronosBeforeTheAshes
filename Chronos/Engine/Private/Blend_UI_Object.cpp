#include "..\Public\Blend_UI_Object.h"

CBlend_UI_Object::CBlend_UI_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject{ pDevice, pContext }
{

}

CBlend_UI_Object::CBlend_UI_Object(const CBlend_UI_Object& Prototype)
	: CBlendObject{ Prototype }
{

}

HRESULT CBlend_UI_Object::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlend_UI_Object::Initialize(void* pArg)
{
	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

	/* 유아이의 초기 위치와 사이즈를 설정한다. */
	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	_uint		iNumView = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumView, &ViewportDesc);

	/* m_ViewMatrix : XMFLOAT4X4 */
	/* XMMatrixIdentity함수의 리턴값 XMMATRIX */
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_fViewWidth = ViewportDesc.Width;
	m_fViewHeight = ViewportDesc.Height;

	// XMVector3Equal(XMLoadFloat3(&m_vTmp), XMLoadFloat3(&m_vTmp));	

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

_uint CBlend_UI_Object::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CBlend_UI_Object::Update(_float fTimeDelta)
{
}

void CBlend_UI_Object::Late_Update(_float fTimeDelta)
{
	Compute_ViewZ();

	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - m_fViewWidth * 0.5f, -m_fY + m_fViewHeight * 0.5f, 0.f, 1.f));
}

HRESULT CBlend_UI_Object::Render()
{
	return S_OK;
}

_bool CBlend_UI_Object::On_MousePoint(POINT ptMouse)
{
	if (m_fX - (m_fSizeX * 0.5f) < ptMouse.x && ptMouse.x < m_fX + (m_fSizeX * 0.5f)
		&& m_fY - (m_fSizeY * 0.5f) < ptMouse.y && ptMouse.y < m_fY + (m_fSizeY * 0.5f))
		return true;

	return false;
}



void CBlend_UI_Object::Free()
{
	__super::Free();

}

void CBlend_UI_Object::Return()
{
	__super::Return();
}
