#include "stdafx.h"
#include "Pedestal_Item.h"
#include "GameInstance.h"

#include "Item.h"

CPedestal_Item::CPedestal_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CPedestal_Item::CPedestal_Item(const CPedestal_Item& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CPedestal_Item::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPedestal_Item::Initialize(void* pArg)
{
	PEDESTAL_ITEM_DESC* pDesc = static_cast<PEDESTAL_ITEM_DESC*>(pArg);
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);

	m_fOffset = pDesc->fOffset;
	m_ppItem = pDesc->ppItem;

	Set_Position(XMVectorSetY(Get_Position(), XMVectorGetY(Get_Position()) + m_fOffset));
	m_fSpeed = 0.5f;

	return S_OK;
}

_uint CPedestal_Item::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CPedestal_Item::Update(_float fTimeDelta)
{
	if (nullptr == *m_ppItem)
		return;

	_vector vPos = Get_Position();
	if (XMVectorGetY(vPos) < 4.75f)
		m_fSpeed += fTimeDelta * 0.5f;
	else
		m_fSpeed -= fTimeDelta * 0.5f;

	if (0.5f < m_fSpeed)
		m_fSpeed = 0.5f;
	else if (-0.5f > m_fSpeed)
		m_fSpeed = -0.5f;

	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + m_fSpeed * fTimeDelta);

	if (XMVectorGetY(vPos) < 4.5f)
		vPos = XMVectorSetY(vPos, 4.5f);
	else if (XMVectorGetY(vPos) > 5.f)
		vPos = XMVectorSetY(vPos, 5.f);

	Set_Position(vPos);

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));
}

void CPedestal_Item::Late_Update(_float fTimeDelta)
{
	if (nullptr == *m_ppItem)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPedestal_Item::Render()
{
	if(nullptr != *m_ppItem)
		(*m_ppItem)->Render(m_WorldMatrix);

	return S_OK;
}

HRESULT CPedestal_Item::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

CPedestal_Item* CPedestal_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPedestal_Item* pInstance = new CPedestal_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CPedestal_Item"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPedestal_Item::Clone(void* pArg)
{
	CPedestal_Item* pInstance = new CPedestal_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CPedestal_Item"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CPedestal_Item::Pooling()
{
	return new CPedestal_Item(*this);
}

void CPedestal_Item::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
