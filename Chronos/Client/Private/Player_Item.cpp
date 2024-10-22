#include "stdafx.h"
#include "Player_Item.h"
#include "GameInstance.h"

#include "Item.h"
#include "Inventory.h"

CPlayer_Item::CPlayer_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Part(pDevice, pContext)
{
}

CPlayer_Item::CPlayer_Item(const CPlayer_Item& Prototype)
	: CPlayer_Part(Prototype)
{
}

void CPlayer_Item::Set_Item(const _wstring strItemKey)
{
	if (nullptr != m_pItem)
		Release_Item();

	m_pItem = m_pInventory->Find_Item(strItemKey);
	if (nullptr == m_pItem)
		return;

	Safe_AddRef(m_pItem);
}

HRESULT CPlayer_Item::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Item::Initialize(void* pArg)
{
	PLAYER_ITEM_DESC* pDesc = static_cast<PLAYER_ITEM_DESC*>(pArg);

	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
	m_pInventory = pDesc->pInventory;
	Safe_AddRef(m_pInventory);

	return S_OK;
}

_uint CPlayer_Item::Priority_Update(_float fTimeDelta)
{
	if (nullptr != m_pItem)
		m_pItem->Priority_Update(fTimeDelta);

	return OBJ_NOEVENT;
}

void CPlayer_Item::Update(_float fTimeDelta)
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


	if (nullptr != m_pItem)
		m_pItem->Update(fTimeDelta);
}

void CPlayer_Item::Late_Update(_float fTimeDelta)
{
	if (nullptr != m_pItem)
		m_pItem->Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPlayer_Item::Render()
{
	if (nullptr != m_pItem)
		m_pItem->Render(m_WorldMatrix);

	return S_OK;
}

HRESULT CPlayer_Item::Ready_Components()
{
	return S_OK;
}

CPlayer_Item* CPlayer_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Item* pInstance = new CPlayer_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_Item"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Item::Clone(void* pArg)
{
	CPlayer_Item* pInstance = new CPlayer_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CPlayer_Item"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CPlayer_Item::Pooling()
{
	return new CPlayer_Item(*this);
}
void CPlayer_Item::Free()
{
	__super::Free();

	Safe_Release(m_pItem);
	Safe_Release(m_pInventory);
}
