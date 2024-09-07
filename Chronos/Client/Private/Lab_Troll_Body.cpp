#include "stdafx.h"
#include "Lab_Troll_Body.h"
#include "GameInstance.h"

CLab_Troll_Body::CLab_Troll_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{
}

CLab_Troll_Body::CLab_Troll_Body(const CLab_Troll_Body& Prototype)
    : CPartObject(Prototype)
{
}

const _float4x4* CLab_Troll_Body::Get_BoneMatrix_Ptr(const _char* pBoneName) const
{
    return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(pBoneName);
}

HRESULT CLab_Troll_Body::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLab_Troll_Body::Initialize(void* pArg)
{
    BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);

    __super::Initialize(pDesc);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pNavigationCom = pDesc->pNavigationCom;
    Safe_AddRef(m_pNavigationCom);

    m_pConstruct_TransformCom = pDesc->pConstruct_TransformCom;
    Safe_AddRef(m_pConstruct_TransformCom);

    m_pTrollAnim = pDesc->pTrollAnim;
    m_pIsFinished = pDesc->pIsFinished;

    return S_OK;
}

_uint CLab_Troll_Body::Priority_Update(_float fTimeDelta)
{

    return OBJ_NOEVENT;
}

void CLab_Troll_Body::Update(_float fTimeDelta)
{
    m_pModelCom->SetUp_Animation(*m_pTrollAnim);
    Play_Animation(fTimeDelta);
}

void CLab_Troll_Body::Late_Update(_float fTimeDelta)
{
    XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLab_Troll_Body::Render()
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

HRESULT CLab_Troll_Body::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Lab_Troll"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

void CLab_Troll_Body::Play_Animation(_float fTimeDelta)
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

CLab_Troll_Body* CLab_Troll_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLab_Troll_Body* pInstance = new CLab_Troll_Body(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CLab_Troll_Body"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLab_Troll_Body::Clone(void* pArg)
{
    CLab_Troll_Body* pInstance = new CLab_Troll_Body(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CLab_Troll_Body"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CLab_Troll_Body::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pConstruct_TransformCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
