#include "Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CLight::CLight()
{
}

void CLight::Set_Pos(_fvector vPos)
{
	XMStoreFloat4(&m_LightDesc.vPosition, vPos);
}

void CLight::Set_Range(_float fRange)
{
	m_LightDesc.fRange = fRange;
}

void CLight::Set_Diffuse(_fvector vColor)
{
	XMStoreFloat4(&m_LightDesc.vDiffuse, vColor);
	XMStoreFloat4(&m_LightDesc.vAmbient, vColor * 0.4f);
	m_LightDesc.vSpecular = m_LightDesc.vDiffuse;
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	_uint		iPassIndex = { 0 };

	if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;

		iPassIndex = 1;
	}
	else
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;

		iPassIndex = 2;
	}

	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

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
