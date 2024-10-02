#include "DragonHeart.h"

CDragonHeart::CDragonHeart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CDragonHeart::CDragonHeart(const CDragonHeart& Prototype)
	: CItem(Prototype)
{
}

HRESULT CDragonHeart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDragonHeart::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

_uint CDragonHeart::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CDragonHeart::Update(_float fTimeDelta)
{
}

void CDragonHeart::Late_Update(_float fTimeDelta)
{
}

HRESULT CDragonHeart::Render()
{
	return S_OK;
}

_bool CDragonHeart::Use_Item()
{
	return __super::Use_Item();
}

HRESULT CDragonHeart::Ready_Components()
{
	return S_OK;
}

CDragonHeart* CDragonHeart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDragonHeart* pInstance = new CDragonHeart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CDragonHeart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDragonHeart::Clone(void* pArg)
{
	CDragonHeart* pInstance = new CDragonHeart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CDragonHeart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDragonHeart::Free()
{
	__super::Free();
}

