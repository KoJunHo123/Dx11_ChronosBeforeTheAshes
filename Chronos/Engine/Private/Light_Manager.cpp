#include "Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iIndex) const
{
    auto	iter = m_Lights.begin();

    for (size_t i = 0; i < iIndex; i++)
        ++iter;

    return (*iter)->Get_LightDesc();
}

void CLight_Manager::Set_Pos(_uint iIndex, _fvector vPos)
{
    auto	iter = m_Lights.begin();

    for (size_t i = 0; i < iIndex; i++)
        ++iter;

    (*iter)->Set_Pos(vPos);
}

void CLight_Manager::Set_Diffuse(_uint iIndex, _fvector vColor)
{
    auto	iter = m_Lights.begin();

    for (size_t i = 0; i < iIndex; i++)
        ++iter;

    (*iter)->Set_Diffuse(vColor);
}

void CLight_Manager::Set_Range(_uint iIndex, _float fRange)
{
    auto	iter = m_Lights.begin();

    for (size_t i = 0; i < iIndex; i++)
        ++iter;

    (*iter)->Set_Range(fRange);

}

HRESULT CLight_Manager::Initialize()
{
    return S_OK;
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
    CLight* pLight = CLight::Create(LightDesc);

    if (nullptr == pLight)
        return E_FAIL;

    m_Lights.push_back(pLight);

    return S_OK;
}

HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    // 해당하는 빛 연산이 들어간 그림을 한장씩 그리고, 기존에 그려졌던 거 위로 블렌딩.
    for (auto& pLight : m_Lights)
        pLight->Render(pShader, pVIBuffer);

    return S_OK;
}

CLight_Manager* CLight_Manager::Create()
{
    CLight_Manager* pInstance = new CLight_Manager();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Create Failed : CLight_Manager"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CLight_Manager::Free()
{
    for (auto& Light : m_Lights)
        Safe_Release(Light);
    m_Lights.clear();
}
