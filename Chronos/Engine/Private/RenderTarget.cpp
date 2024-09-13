#include "RenderTarget.h"

CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice (pDevice)
	, m_pContext (pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	D3D11_TEXTURE2D_DESC TextureDesc = {};
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	// 이거 벡버퍼 가져와서 렌더타겟뷰랑 셰이더 리소스 뷰 만들기 위한 텍스처임.
	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;	// 밉맵, 멀티샘플링 이런거 필요x.
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	// 텍스처 만들고
	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	// 만든 텍스처로 렌더 타겟 뷰 만들고
	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	// 만든 텍스처로 셰이더 리소스 뷰 만들고.
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iWidth, iHeight, ePixelFormat, vClearColor)))
	{
		MSG_BOX(TEXT("Create Failed : CRenderTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pTexture2D);
	Safe_Release(m_pRTV);
	Safe_Release(m_pSRV);
}
