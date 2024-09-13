#include "Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CLight::CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	_uint iPassIndex = { 0 };

	if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4));
		iPassIndex = 1;
	}
	else
	{
		iPassIndex = 2;
	}

	pShader->Begin(iPassIndex);

	pVIBuffer->Bind_Buffers();
	pVIBuffer->Render();

	return S_OK;
}

CLight* CLight::Create(const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight();
	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX(TEXT("Create Failed : CLight"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLight::Free()
{
	__super::Free();

}
