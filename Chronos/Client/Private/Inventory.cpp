#include "stdafx.h"
#include "Inventory.h"
#include "Item.h"

CInventory::CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CInventory::CInventory(const CInventory& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CInventory::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInventory::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

_uint CInventory::Priority_Update(_float fTimeDelta)
{
    return OBJ_NOEVENT;
}

void CInventory::Update(_float fTimeDelta)
{
}

void CInventory::Late_Update(_float fTimeDelta)
{
}

HRESULT CInventory::Render()
{
    return S_OK;
}


void CInventory::Add_Item(const _wstring strItemKey, CItem* pAddItem)
{
    CItem* pItem = Find_Item(strItemKey);
    if (nullptr == pItem)
    {
        m_Items.emplace(strItemKey, pItem);
        return;
    }

    pItem->Add_Item();
}

CItem* CInventory::Find_Item(const _wstring strItemKey)
{
    auto& iter = m_Items.find(strItemKey);

    if (iter == m_Items.end())
        return nullptr;

    return iter->second;
}

HRESULT CInventory::Ready_Components()
{


    return S_OK;
}

CInventory* CInventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInventory* pInstance = new CInventory(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CInventory"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInventory::Clone(void* pArg)
{
    CInventory* pInstance = new CInventory(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CInventory"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInventory::Free()
{
    __super::Free();

    for (auto& Pair : m_Items)
        Safe_Release(Pair.second);
}
