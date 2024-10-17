#include "stdafx.h"
#include "..\Public\UI_PuzzleExit.h"
#include "GameInstance.h"

#include "Player.h"
#include "Camera_Container.h"
#include "Camera_Shorder.h"

CUI_PuzzleExit::CUI_PuzzleExit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlend_UI_Object(pDevice, pContext)
{
}

CUI_PuzzleExit::CUI_PuzzleExit(const CUI_PuzzleExit& Prototype)
	: CBlend_UI_Object(Prototype)
{
}

HRESULT CUI_PuzzleExit::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_PuzzleExit::Initialize(void* pArg)
{
	UI_PUZZLEEXIT_DESC* pDesc = static_cast<UI_PUZZLEEXIT_DESC*>(pArg);

	m_pPlayer = pDesc->pPlayer;
	Safe_AddRef(m_pPlayer);


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

_uint CUI_PuzzleExit::Priority_Update(_float fTimeDelta)
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

void CUI_PuzzleExit::Update(_float fTimeDelta)
{



}

void CUI_PuzzleExit::Late_Update(_float fTimeDelta)
{
	if (true == m_bOn)
	{
		__super::Late_Update(fTimeDelta);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLENDUI, this);
	}
}

HRESULT CUI_PuzzleExit::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Text(TEXT("Font_145"), TEXT("E"), XMVectorSet(m_fX, m_fY, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.5f, true)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Text(TEXT("Font_145"), TEXT("µÚ·Î"), XMVectorSet(m_fX + 100.f, m_fY, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.5f, true)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PuzzleExit::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Keyboard"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_PuzzleExit* CUI_PuzzleExit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_PuzzleExit* pInstance = new CUI_PuzzleExit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_PuzzleExit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_PuzzleExit::Clone(void* pArg)
{
	CUI_PuzzleExit* pInstance = new CUI_PuzzleExit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_PuzzleExit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PuzzleExit::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pPlayer);
}
