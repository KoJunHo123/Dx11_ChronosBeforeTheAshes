#include "stdafx.h"
#include "UI_DragonStone.h"
#include "GameInstance.h"

#include "Player.h"


CUI_DragonStone::CUI_DragonStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

CUI_DragonStone::CUI_DragonStone(const CUI_DragonStone& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUI_DragonStone::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_DragonStone::Initialize(void* pArg)
{
	UI_DRAGONSTONE_DESC* pDesc = static_cast<UI_DRAGONSTONE_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pPlayer = pDesc->pPlayer;
	Safe_AddRef(m_pPlayer);

	return S_OK;
}

_uint CUI_DragonStone::Priority_Update(_float fTimeDelta)
{

	return OBJ_NOEVENT;
}

void CUI_DragonStone::Update(_float fTimeDelta)
{
	m_iCurrentSkill = m_pPlayer->Get_Skill();
}

void CUI_DragonStone::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_DragonStone::Render()
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
	if(CPlayer::SKILL_RED == m_iCurrentSkill)
	{
		if (FAILED(m_pCoreTextureCom_Red->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	}
	else if (CPlayer::SKILL_PUPPLE == m_iCurrentSkill)
	{
		if (FAILED(m_pCoreTextureCom_Pupple->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_DragonStone::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture_Border */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DragonStone_Border"),
		TEXT("Com_Texture_Border"), reinterpret_cast<CComponent**>(&m_pBorderTextureCom))))
		return E_FAIL;

	/* FOR.Com_Texture_Core */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DragonStone_Core_Red"),
		TEXT("Com_Texture_Core_Red"), reinterpret_cast<CComponent**>(&m_pCoreTextureCom_Red))))
		return E_FAIL;

	/* FOR.Com_Texture_Core */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_DragonStone_Core_Pupple"),
		TEXT("Com_Texture_Core_Pupple"), reinterpret_cast<CComponent**>(&m_pCoreTextureCom_Pupple))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_DragonStone* CUI_DragonStone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_DragonStone* pInstance = new CUI_DragonStone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CUI_DragonStone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_DragonStone::Clone(void* pArg)
{
	CUI_DragonStone* pInstance = new CUI_DragonStone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CUI_DragonStone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_DragonStone::Free()
{
	__super::Free();

	Safe_Release(m_pBorderTextureCom);
	Safe_Release(m_pCoreTextureCom_Red);
	Safe_Release(m_pCoreTextureCom_Pupple);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pPlayer);
}
