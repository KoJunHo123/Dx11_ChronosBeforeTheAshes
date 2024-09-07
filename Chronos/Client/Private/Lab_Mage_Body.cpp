#include "stdafx.h"
#include "Lab_Mage_Body.h"
#include "GameInstance.h"

CLab_Mage_Body::CLab_Mage_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{
}

CLab_Mage_Body::CLab_Mage_Body(const CLab_Mage_Body& Prototype)
    : CPartObject(Prototype)
{
}

HRESULT CLab_Mage_Body::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLab_Mage_Body::Initialize(void* pArg)
{
    BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);

    __super::Initialize(pDesc);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pNavigationCom = pDesc->pNavigationCom;
    Safe_AddRef(m_pNavigationCom);

    m_pConstruct_TransformCom = pDesc->pConstruct_TransformCom;
    Safe_AddRef(m_pConstruct_TransformCom);

    m_pMageAnim = pDesc->pMageAnim;
    m_pIsFinished = pDesc->pIsFinished;

    return S_OK;
}

_uint CLab_Mage_Body::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CLab_Mage_Body::Update(_float fTimeDelta)
{
    m_pModelCom->SetUp_Animation(*m_pMageAnim);
    Play_Animation(fTimeDelta);
}

void CLab_Mage_Body::Late_Update(_float fTimeDelta)
{
    XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLab_Mage_Body::Render()
{
    if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;
        /*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
            return E_FAIL;*/

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLab_Mage_Body::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Lab_Mage"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

void CLab_Mage_Body::Play_Animation(_float fTimeDelta)
{
    _vector vStateChange{};

    *m_pIsFinished = m_pModelCom->Play_Animation(fTimeDelta, vStateChange);

    vStateChange = m_pConstruct_TransformCom->Get_Rotated_Vector(vStateChange);

    _vector vBossPos = m_pConstruct_TransformCom->Get_State(CTransform::STATE_POSITION);

    _vector vMovePosition = vBossPos + vStateChange;

    _vector vLine = {};

    if (nullptr == m_pNavigationCom || true == m_pNavigationCom->isMove(vMovePosition, &vLine))
        m_pConstruct_TransformCom->Set_State(CTransform::STATE_POSITION, vMovePosition);
    else
    {
        vLine = XMVector3Normalize(vLine);

        _float fDot = XMVectorGetX(XMVector3Dot(vLine, vStateChange));

        if (fDot < 0.f)
        {
            vLine *= -1.f;
            fDot = XMVectorGetX(XMVector3Dot(vLine, vStateChange));
        }

        vStateChange = vLine * fDot;
        vMovePosition = vBossPos + vStateChange;

        if (true == m_pNavigationCom->isMove(vMovePosition, &vLine))
            m_pConstruct_TransformCom->Set_State(CTransform::STATE_POSITION, vMovePosition);
    }

}

CLab_Mage_Body* CLab_Mage_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLab_Mage_Body* pInstance = new CLab_Mage_Body(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CLab_Mage_Body"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLab_Mage_Body::Clone(void* pArg)
{
    CLab_Mage_Body* pInstance = new CLab_Mage_Body(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CLab_Mage_Body"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CLab_Mage_Body::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pConstruct_TransformCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
