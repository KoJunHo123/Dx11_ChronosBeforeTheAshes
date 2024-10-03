#include "Item.h"

CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CItem::CItem(const CItem& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iItemCount = 1;

	return S_OK;
}

_uint CItem::Priority_Update(_float fTimeDelta)
{

	return OBJ_NOEVENT;
}

void CItem::Update(_float fTimeDelta)
{
}

void CItem::Late_Update(_float fTimeDelta)
{
}

HRESULT CItem::Render(const _float4x4& WorldMatrix)
{
	return S_OK;
}

_bool CItem::Use_Item(class CPlayer* pPlayer)
{
	if (0 == m_iItemCount)
		return false;

	--m_iItemCount;
	return true;
}

void CItem::Free()
{
	__super::Free();
}
