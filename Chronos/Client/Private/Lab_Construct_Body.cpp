#include "stdafx.h"
#include "Lab_Construct_Body.h"
#include "GameInstance.h"

#include "Lab_Construct.h"

CLab_Construct_Body::CLab_Construct_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{
}

CLab_Construct_Body::CLab_Construct_Body(const CLab_Construct_Body& Prototype)
    : CPartObject(Prototype)
{
}


HRESULT CLab_Construct_Body::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLab_Construct_Body::Initialize(void* pArg)
{
    BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);

    __super::Initialize(pDesc);

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pNavigationCom = pDesc->pNavigationCom;
    Safe_AddRef(m_pNavigationCom);

    m_pConstruct_TransformCom = pDesc->pConstruct_TransformCom;
    Safe_AddRef(m_pConstruct_TransformCom);

    m_pState = pDesc->pState;
    m_pIsFinished = pDesc->pIsFinished;
    m_pHP = pDesc->pHP;
    m_pDistance = pDesc->pDistance;

    m_eConstructAnim = LAB_CONSTRUCT_IDLE;

    m_fSpeed = 3.f;

    return S_OK;
}

_uint CLab_Construct_Body::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CLab_Construct_Body::Update(_float fTimeDelta)
{

    cout << m_eConstructAnim << endl;
    if (CLab_Construct::STATE_IDLE == *m_pState)
    {
        m_eConstructAnim = LAB_CONSTRUCT_IDLE;
    }
    else if (CLab_Construct::STATE_ATTACK == *m_pState)
    {
        if(false == m_bAnimStart)
        {
            if (*m_pDistance < 5.f)
                m_eConstructAnim = LAB_CONSTRUCT_ATK_SHIELDSWIPE;
            else if (*m_pDistance < 8.f)
                m_eConstructAnim = LAB_CONSTRUCT_ATK_DOWNSWIPE;
            else if (*m_pDistance < 12.f)
                m_eConstructAnim = LAB_CONSTRUCT_ATK_VERTICALSLAM;
            else if (*m_pDistance < 15.f)
                m_eConstructAnim = LAB_CONSTRUCT_ATK_3HIT_COMBO;

            m_bAnimStart = true;
        }
    }
    else if (CLab_Construct::STATE_WALK == *m_pState)
    {
        if (*m_pDistance < 8.f)
        {
            m_eConstructAnim = LAB_CONSTRUCT_WALK_B;
            m_pConstruct_TransformCom->Go_Backward(fTimeDelta * m_fSpeed, m_pNavigationCom);
        }
        else if (*m_pDistance < 12.f)
        {
            if(LAB_CONSTRUCT_WALK_L != m_eConstructAnim && LAB_CONSTRUCT_WALK_R != m_eConstructAnim )
            {
                if(m_pGameInstance->Get_Random_Normal() < 0.5f)
                    m_eConstructAnim = LAB_CONSTRUCT_WALK_L;
                else
                    m_eConstructAnim = LAB_CONSTRUCT_WALK_R;
            }
            if(LAB_CONSTRUCT_WALK_L == m_eConstructAnim)
                m_pConstruct_TransformCom->Go_Left(fTimeDelta * m_fSpeed, m_pNavigationCom);
            else if(LAB_CONSTRUCT_WALK_R == m_eConstructAnim)
                m_pConstruct_TransformCom->Go_Right(fTimeDelta * m_fSpeed, m_pNavigationCom);
        }
        else
        {
            m_eConstructAnim = LAB_CONSTRUCT_WALK_F;
            m_pConstruct_TransformCom->Go_Straight(fTimeDelta * m_fSpeed, m_pNavigationCom);
        }
    }
    else if (CLab_Construct::STATE_IMPACT == *m_pState)
    {
        if (90 > abs(m_fHittedAngle))
            m_eConstructAnim = LAB_CONSTRUCT_IMPACT_F_SHORT;
        else
            m_eConstructAnim = LAB_CONSTRUCT_IMPACT_B;
    }
    
    m_pModelCom->SetUp_Animation(m_eConstructAnim, Animation_Loop(), Animation_NonInterpolate());
    Play_Animation(fTimeDelta *  1.2f);

    if (true == *m_pIsFinished && CLab_Construct::STATE_ATTACK == *m_pState)
        m_bAnimStart = false;
}

void CLab_Construct_Body::Late_Update(_float fTimeDelta)
{
    XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CLab_Construct_Body::Render()
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

void CLab_Construct_Body::Reset_Animation()
{
    m_pModelCom->Reset_Animation();
}

_bool CLab_Construct_Body::Animation_Loop()
{
    if (LAB_CONSTRUCT_IDLE == m_eConstructAnim
        || LAB_CONSTRUCT_WALK_F == m_eConstructAnim
        || LAB_CONSTRUCT_WALK_B == m_eConstructAnim
        || LAB_CONSTRUCT_WALK_L == m_eConstructAnim
        || LAB_CONSTRUCT_WALK_R == m_eConstructAnim)
        return true;

    return false;
}

_bool CLab_Construct_Body::Animation_NonInterpolate()
{
    if (LAB_CONSTRUCT_IMPACT_B == m_eConstructAnim ||
        LAB_CONSTRUCT_IMPACT_DEATH == m_eConstructAnim ||
        LAB_CONSTRUCT_IMPACT_F_SHORT == m_eConstructAnim)
        return true;

    return false;
}

HRESULT CLab_Construct_Body::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Lab_Construct"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

void CLab_Construct_Body::Play_Animation(_float fTimeDelta)
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

CLab_Construct_Body* CLab_Construct_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLab_Construct_Body* pInstance = new CLab_Construct_Body(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CLab_Construct_Body"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLab_Construct_Body::Clone(void* pArg)
{
    CLab_Construct_Body* pInstance = new CLab_Construct_Body(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CLab_Construct_Body"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CLab_Construct_Body::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pConstruct_TransformCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
