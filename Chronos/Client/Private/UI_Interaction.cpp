#include "stdafx.h"
#include "UI_Interaction.h"
#include "GameInstance.h"

#include "Player.h"

CUI_Interaction::CUI_Interaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

CUI_Interaction::CUI_Interaction(const CUI_Interaction& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUI_Interaction::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Interaction::Initialize(void* pArg)
{
	UI_INTERACTION_DESC* pDesc = static_cast<UI_INTERACTION_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pPlayer = pDesc->pPlayer;
	Safe_AddRef(m_pPlayer);

	return S_OK;
}

_uint CUI_Interaction::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
		return OBJ_DEAD;

	m_bOn = m_pPlayer->Get_Interaction();

	return OBJ_NOEVENT;
}

void CUI_Interaction::Update(_float fTimeDelta)
{
	if(true == m_bOn)
	{
		_vector vPos = m_pPlayer->Get_Position();
		_vector vRight = m_pPlayer->Get_State(CTransform::STATE_RIGHT);

		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 3.f);
		vPos += XMVector3Normalize(vRight) * 2.f;

		vPos = XMVector3TransformCoord(vPos, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vPos = XMVector3TransformCoord(vPos, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
		m_fX = XMVectorGetX(vPos) * 0.5f + 0.5f;
		m_fY = XMVectorGetY(vPos) * -0.5f + 0.5f;
		m_fZ = XMVectorGetZ(vPos);

		m_fX *= g_iWinSizeX;
		m_fY *= g_iWinSizeY;

		m_fSizeX = 40.f;
		m_fSizeY = 40.f;
	}
}

void CUI_Interaction::Late_Update(_float fTimeDelta)
{
	if(true == m_bOn)
	{
		__super::Late_Update(fTimeDelta);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
	}
}

HRESULT CUI_Interaction::Render()
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


	return S_OK;
}

HRESULT CUI_Interaction::Ready_Components()
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

CUI_Interaction* CUI_Interaction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Interaction* pInstance = new CUI_Interaction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CUI_Interaction"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Interaction::Clone(void* pArg)
{
	CUI_Interaction* pInstance = new CUI_Interaction(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CUI_Interaction"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Interaction::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pPlayer);
}
