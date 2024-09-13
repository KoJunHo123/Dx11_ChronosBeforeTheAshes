#pragma once

#include "Base.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
private:
	CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTarget_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag);
	HRESULT End_MRT();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	// 렌더타겟들
	map<const _wstring, class CRenderTarget*> m_RenderTargets;
	// 위에 저장한 렌더타겟들 중에 장치에 한번에 바인딩 할 렌더타겟들의 묶음.
	map<const _wstring, list<class CRenderTarget*>> m_MRTs;

	// 장치에서 백버퍼, 스텐실 버퍼 받아오는 용도.
	ID3D11RenderTargetView* m_pBackBufferView = { nullptr };
	ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };

private:
	CRenderTarget* Find_RenderTarget(const _wstring& strTargetTag);
	list<class CRenderTarget*>* Find_MRT(const _wstring& strMRTTag);

public:
	static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END