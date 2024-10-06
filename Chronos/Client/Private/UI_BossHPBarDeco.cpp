#include "stdafx.h"
#include "UI_BossHPBarDeco.h"
#include "GameInstance.h"

#include "Monster.h"

CUI_BossHPBarDeco::CUI_BossHPBarDeco(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

CUI_BossHPBarDeco::CUI_BossHPBarDeco(const CUI_BossHPBarDeco& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUI_BossHPBarDeco::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_BossHPBarDeco::Initialize(void* pArg)
{
	UI_HPBARDECO_DESC* pDesc = static_cast<UI_HPBARDECO_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pMonster = pDesc->pMonster;
	Safe_AddRef(m_pMonster);

	return S_OK;
}

_uint CUI_BossHPBarDeco::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CUI_BossHPBarDeco::Update(_float fTimeDelta)
{
}

void CUI_BossHPBarDeco::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (true == m_pMonster->Get_Dead())
		m_bDead = true;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_BossHPBarDeco::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	// Ã¼·Â¹Ù
	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_BossHPBarDeco::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Boss_BarDeco"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_BossHPBarDeco* CUI_BossHPBarDeco::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BossHPBarDeco* pInstance = new CUI_BossHPBarDeco(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CUI_BossHPBarDeco"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_BossHPBarDeco::Clone(void* pArg)
{
	CUI_BossHPBarDeco* pInstance = new CUI_BossHPBarDeco(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CUI_BossHPBarDeco"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BossHPBarDeco::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pMonster);
}
