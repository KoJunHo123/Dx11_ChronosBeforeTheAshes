#include "stdafx.h"
#include "UI_GageBar.h"
#include "GameInstance.h"

#include "Player.h"

CUI_GageBar::CUI_GageBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

CUI_GageBar::CUI_GageBar(const CUI_GageBar& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUI_GageBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_GageBar::Initialize(void* pArg)
{
	UI_GAGEBAR_DESC* pDesc = static_cast<UI_GAGEBAR_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pPlayer = pDesc->pPlayer;
	Safe_AddRef(m_pPlayer);

	m_eType = pDesc->eType;

	switch (m_eType)
	{
	case TYPE_HP:
		m_fMaxGage = m_pPlayer->Get_MaxHP();
		m_vColor = { 1.f, 0.f, 0.f , 1.f };
		break;

	case TYPE_STAMINA:
		m_fMaxGage = m_pPlayer->Get_MaxStamina();
		m_vColor = { 1.f, 1.f, 1.f, 1.f };
		break;

	case TYPE_SKILL:
		m_fMaxGage = m_pPlayer->Get_MaxSkillGage();
		m_vColor = { 0.f, 0.f, 0.f, 0.f };
		break;
	}

	return S_OK;
}

_uint CUI_GageBar::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CUI_GageBar::Update(_float fTimeDelta)
{
	switch (m_eType)
	{
	case TYPE_HP:
		m_fGage = m_pPlayer->Get_HP();
		break;

	case TYPE_STAMINA:
		m_fGage = m_pPlayer->Get_Stamina();
		break;

	case TYPE_SKILL:
		m_fGage = m_pPlayer->Get_SkillGage();
		{
			_uint iSkillIndex = m_pPlayer->Get_Skill();
			switch (iSkillIndex)
			{
			case CPlayer::SKILL_RED:
				m_vColor = { 0.863f, 0.078f, 0.235f, 1.f };
				break;

			case CPlayer::SKILL_PUPPLE:
				m_vColor = { 0.541f, 0.169f, 0.886f, 1.f };
				break;
			}
		}

		break;
	}

	m_fRatio = m_fGage / m_fMaxGage;
}

void CUI_GageBar::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUI_GageBar::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pBorderTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pCoreTextureCom->Bind_ShadeResource(m_pShaderCom, "g_PackTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &m_fRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_GageBar::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture_Border */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_GageBar_Border"),
		TEXT("Com_Texture_Border"), reinterpret_cast<CComponent**>(&m_pBorderTextureCom))))
		return E_FAIL;

	/* FOR.Com_Texture_Core */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Pack"),
		TEXT("Com_Texture_Core"), reinterpret_cast<CComponent**>(&m_pCoreTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_GageBar* CUI_GageBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_GageBar* pInstance = new CUI_GageBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CUI_GageBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_GageBar::Clone(void* pArg)
{
	CUI_GageBar* pInstance = new CUI_GageBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CUI_GageBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_GageBar::Pooling()
{
	return new CUI_GageBar(*this);
}

void CUI_GageBar::Free()
{
	__super::Free();

	Safe_Release(m_pBorderTextureCom);
	Safe_Release(m_pCoreTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pPlayer);
}
