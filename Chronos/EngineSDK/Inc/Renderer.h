#pragma once

#include "Base.h"

/* 1. 객체들을 그리는 순서에 따라서 보관하고. 정해진 순서대로 객체들의 렌더함수를 호출한다.  */
/* 2. 3D라고 하더라도 객체들을 그리는 순서가 상당히 중요하다. */
/* 3. 하늘, 블렌딩, UI관련된 객체들은 그리는 순서의 관리가 필수적으로 필요하다. */

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RG_PRIORITY, RG_SHADOWOBJ, RG_NONBLEND, RG_NONLIGHT, RG_BLEND, RG_UI, RG_BLENDUI, RG_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;
	
		
public:
	HRESULT Initialize(_uint iWinSizeX, _uint iWinSizeY);
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroupID, class CGameObject* pRenderObject);
	HRESULT Draw();

	HRESULT Bind_DefaultTexture(class CShader* pShader, const _char* pConstantName);

	_bool FadeIn(_float fTimeDelta);
	_bool FadeOut(_float fTimeDelta);

#ifdef _DEBUG

public:
	HRESULT Add_DebugObject(class CComponent* pDebugObject) {
		Safe_AddRef(pDebugObject);
		m_DebugObjects.push_back(pDebugObject);
		return S_OK;
	}
#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	ID3D11DepthStencilView* m_pLightDepthStencilView = { nullptr };
	list<class CGameObject*>	m_RenderObjects[RG_END];
	class CGameInstance* m_pGameInstance = { nullptr };

	_uint m_iWinSizeX = {};
	_uint m_iWinSizeY = {};

private:
	class CShader* m_pShader = { nullptr };
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	class CTexture* m_pDefaultTexture = { nullptr };

	_float4x4					m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};

	_float m_fRatio = { 0.f };
	_bool m_bFadeIn = { false };
	_bool m_bFadeOut = { false };

#ifdef _DEBUG
private:
	list<class CComponent*>		m_DebugObjects;
#endif

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_ShadowObj();
	HRESULT Render_Lights();
	HRESULT Render_Deferred();
	HRESULT Render_NonLights();
	HRESULT Render_Blend();
	HRESULT Render_UI();
	HRESULT Render_BlendUI();
	HRESULT Render_Fade();

private:
	HRESULT Ready_LightDepthStencilView();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();

#endif



public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;
};

END