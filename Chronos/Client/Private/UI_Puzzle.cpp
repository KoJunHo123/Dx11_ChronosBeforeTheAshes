#include "stdafx.h"
#include "..\Public\UI_Puzzle.h"
#include "GameInstance.h"

#include "Player.h"
#include "Inventory.h"
#include "Camera_Container.h"
#include "Camera_Shorder.h"

CUI_Puzzle::CUI_Puzzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlend_UI_Object( pDevice, pContext )
{
}

CUI_Puzzle::CUI_Puzzle(const CUI_Puzzle& Prototype)
	: CBlend_UI_Object( Prototype )
{
}

HRESULT CUI_Puzzle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Puzzle::Initialize(void* pArg)
{
	UI_PUZZLE_DESC* pDesc = static_cast<UI_PUZZLE_DESC*>(pArg);

	m_pPlayer = pDesc->pPlayer;
	Safe_AddRef(m_pPlayer);

	m_pInventory = pDesc->pInventory;
	Safe_AddRef(m_pInventory);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

_uint CUI_Puzzle::Priority_Update(_float fTimeDelta)
{
	if (true == m_pPlayer->Get_Interaction())
	{
		m_pCurrentCamera = dynamic_cast<CCamera*>(static_cast<CCamera_Container*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), 0))->Get_PartObject());

		if (CCamera_Container::CAMERA_INTERACTION == m_pCurrentCamera->Get_CameraIndex())
			m_bOn = true;
		else
			m_bOn = false;
	}

	return OBJ_NOEVENT;
}

void CUI_Puzzle::Update(_float fTimeDelta)
{
	if (true == m_bOn)
	{
		if (nullptr == m_pInventory->Find_Item(TEXT("Item_ReplacePuzzle")))
			m_fRatio = 0.5f;
		else
			m_fRatio = 1.f;
	}
}

void CUI_Puzzle::Late_Update(_float fTimeDelta)
{
	if(true == m_bOn)
	{
		__super::Late_Update(fTimeDelta);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLENDUI, this);
	}
}

HRESULT CUI_Puzzle::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &m_fRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(5)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Text(TEXT("Font_145"), TEXT("Tab"), XMVectorSet(m_fX, m_fY, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, m_fRatio), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.5f, true)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Text(TEXT("Font_145"), TEXT("∆€¡Ò ±≥√º"), XMVectorSet(m_fX + 100.f, m_fY, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, m_fRatio), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.5f, true)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Puzzle::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_WideKeyboard"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	
	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_Puzzle* CUI_Puzzle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Puzzle* pInstance = new CUI_Puzzle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Puzzle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Puzzle::Clone(void* pArg)
{
	CUI_Puzzle* pInstance = new CUI_Puzzle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_Puzzle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Puzzle::Pooling()
{
	return new CUI_Puzzle(*this);
}

void CUI_Puzzle::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pPlayer);
	Safe_Release(m_pInventory);
}
