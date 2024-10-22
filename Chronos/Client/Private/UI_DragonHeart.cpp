#include "stdafx.h"
#include "UI_DragonHeart.h"
#include "GameInstance.h"

#include "DragonHeart.h"

CUI_DragonHeart::CUI_DragonHeart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

CUI_DragonHeart::CUI_DragonHeart(const CUI_DragonHeart& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUI_DragonHeart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_DragonHeart::Initialize(void* pArg)
{
	UI_DRAGONHEART_DESC* pDesc = static_cast<UI_DRAGONHEART_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pDragonHeart = pDesc->pDragonHeart;
	Safe_AddRef(m_pDragonHeart);

	return S_OK;
}

_uint CUI_DragonHeart::Priority_Update(_float fTimeDelta)
{

	return OBJ_NOEVENT;
}

void CUI_DragonHeart::Update(_float fTimeDelta)
{
	m_iItemCount = m_pDragonHeart->Get_ItemCount();
	m_iMaxCount = m_pDragonHeart->Get_MaxCount();
}

void CUI_DragonHeart::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_DragonHeart::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	// °¡
	if (FAILED(m_pBorderTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	// ½É
	if (FAILED(m_pCoreTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	_wstring strItemCount = to_wstring(m_iItemCount) + L'/' + to_wstring(m_iMaxCount);
	if (FAILED(m_pGameInstance->Render_Text(TEXT("Font_145"), strItemCount.c_str(), XMVectorSet(m_fX, m_fY + m_fSizeY * 0.25f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.3f, true)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_DragonHeart::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture_Border */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DragonHeart_Border"),
		TEXT("Com_Texture_Border"), reinterpret_cast<CComponent**>(&m_pBorderTextureCom))))
		return E_FAIL;

	/* FOR.Com_Texture_Core */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DragonHeart_Core"),
		TEXT("Com_Texture_Core"), reinterpret_cast<CComponent**>(&m_pCoreTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_DragonHeart* CUI_DragonHeart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_DragonHeart* pInstance = new CUI_DragonHeart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CUI_DragonHeart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_DragonHeart::Clone(void* pArg)
{
	CUI_DragonHeart* pInstance = new CUI_DragonHeart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CUI_DragonHeart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_DragonHeart::Pooling()
{
	return new CUI_DragonHeart(*this);
}

void CUI_DragonHeart::Free()
{
	__super::Free();

	Safe_Release(m_pBorderTextureCom);
	Safe_Release(m_pCoreTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pDragonHeart);
}
