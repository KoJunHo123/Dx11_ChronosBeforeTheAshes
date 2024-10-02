#include "Camera_Interaction.h"

CCamera_Interaction::CCamera_Interaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera(pDevice, pContext)
{
}

CCamera_Interaction::CCamera_Interaction(const CCamera_Interaction& Prototype)
    : CCamera(Prototype)
{
}

HRESULT CCamera_Interaction::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Interaction::Initialize(void* pArg)
{
    CAMERA_INTERACTION_DESC* pDesc = static_cast<CAMERA_INTERACTION_DESC*>(pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

_uint CCamera_Interaction::Priority_Update(_float fTimeDelta)
{
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vTargetPos) + XMLoadFloat3(&m_vCamDir));
    m_pTransformCom->LookAt(XMLoadFloat3(&m_vTargetPos));
 
    __super::Priority_Update(fTimeDelta);
    return OBJ_NOEVENT;
}

void CCamera_Interaction::Update(_float fTimeDelta)
{
}

void CCamera_Interaction::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Interaction::Render()
{
    return S_OK;
}

CCamera_Interaction* CCamera_Interaction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Interaction* pInstance = new CCamera_Interaction(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed: CCamera_Interaction"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Interaction::Clone(void* pArg)
{
    CCamera_Interaction* pInstance = new CCamera_Interaction(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed: CCamera_Interaction"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Interaction::Free()
{
    __super::Free();
}
