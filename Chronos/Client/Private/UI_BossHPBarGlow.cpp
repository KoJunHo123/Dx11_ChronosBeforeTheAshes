#include "stdafx.h"
#include "UI_BossHPBarGlow.h"
#include "GameInstance.h"

#include "Monster.h"

CUI_BossHPBarGlow::CUI_BossHPBarGlow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlend_UI_Object(pDevice, pContext)
{
}

CUI_BossHPBarGlow::CUI_BossHPBarGlow(const CUI_BossHPBarGlow& Prototype)
	: CBlend_UI_Object(Prototype)
{
}

HRESULT CUI_BossHPBarGlow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_BossHPBarGlow::Initialize(void* pArg)
{
	UI_HPBARGLOW_DESC* pDesc = static_cast<UI_HPBARGLOW_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pMonster = pDesc->pMonster;
	Safe_AddRef(m_pMonster);

	return S_OK;
}

_uint CUI_BossHPBarGlow::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CUI_BossHPBarGlow::Update(_float fTimeDelta)
{
}

void CUI_BossHPBarGlow::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (true == m_pMonster->Get_Dead())
		m_bDead = true;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_BLENDUI, this);
}

HRESULT CUI_BossHPBarGlow::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	_float fRatio = 1.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &fRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(5)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Text(TEXT("Font_145"), TEXT("미궁의 수호자"), XMVectorSet(m_fX, m_fY + m_fSizeY * 0.3f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.5f, true)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_BossHPBarGlow::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Glow_Up"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_BossHPBarGlow* CUI_BossHPBarGlow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BossHPBarGlow* pInstance = new CUI_BossHPBarGlow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CUI_BossHPBarGlow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_BossHPBarGlow::Clone(void* pArg)
{
	CUI_BossHPBarGlow* pInstance = new CUI_BossHPBarGlow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CUI_BossHPBarGlow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_BossHPBarGlow::Pooling()
{
	return new CUI_BossHPBarGlow(*this);
}

void CUI_BossHPBarGlow::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pMonster);
}
