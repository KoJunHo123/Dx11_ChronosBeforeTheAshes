#include "stdafx.h"
#include "UI_YouDied.h"
#include "GameInstance.h"

#include "Player.h"

CUI_YouDied::CUI_YouDied(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

CUI_YouDied::CUI_YouDied(const CUI_YouDied& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUI_YouDied::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_YouDied::Initialize(void* pArg)
{
	UI_YOUDIED_DESC* pDesc = static_cast<UI_YOUDIED_DESC*>(pArg);

	m_pPlayer = pDesc->pPlayer;
	Safe_AddRef(m_pPlayer);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

_uint CUI_YouDied::Priority_Update(_float fTimeDelta)
{
	if (true == m_pPlayer->Get_Dead())
	{
		m_bOn = true;
		m_pPlayer->Set_Dead(false);
	}

	return OBJ_NOEVENT;
}

void CUI_YouDied::Update(_float fTimeDelta)
{
	if (true == m_bOn)
	{
		m_fRatio += fTimeDelta;
		if (m_fRatio > 3.f)
		{
			m_bOn = false;
			m_bOff = true;
			m_fRatio = 1.f;
		}
	}
	else if(true == m_bOff)
	{
		m_fRatio -= fTimeDelta;
		if (m_fRatio < 0.f)
		{
			m_fRatio = 0.f;
			m_pPlayer->Set_Revive(true);
			m_bOff = false;
		}
	}

}

void CUI_YouDied::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if(true == m_bOn || true == m_bOff)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_FADE, this);
}

HRESULT CUI_YouDied::Render()
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

	return S_OK;
}

HRESULT CUI_YouDied::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_YouDied"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_YouDied* CUI_YouDied::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_YouDied* pInstance = new CUI_YouDied(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CUI_YouDied"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_YouDied::Clone(void* pArg)
{
	CUI_YouDied* pInstance = new CUI_YouDied(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CUI_YouDied"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_YouDied::Pooling()
{
	return new CUI_YouDied(*this);
}

void CUI_YouDied::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pPlayer);
}
