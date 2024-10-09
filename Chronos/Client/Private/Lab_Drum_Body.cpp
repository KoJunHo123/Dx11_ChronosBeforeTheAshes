#include "stdafx.h"
#include "Lab_Drum_Body.h"
#include "GameInstance.h"

#include "Lab_Drum.h"

CLab_Drum_Body::CLab_Drum_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{
}

CLab_Drum_Body::CLab_Drum_Body(const CLab_Drum_Body& Prototype)
    : CPartObject(Prototype)
{
}


HRESULT CLab_Drum_Body::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLab_Drum_Body::Initialize(void* pArg)
{
    BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);

    if(FAILED(__super::Initialize(pDesc)))
        return E_FAIL;


    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pNavigationCom = pDesc->pNavigationCom;
    Safe_AddRef(m_pNavigationCom);

    m_pConstruct_TransformCom = pDesc->pConstruct_TransformCom;
    Safe_AddRef(m_pConstruct_TransformCom);

    m_pNoiseTextureCom = pDesc->pNoiseTextureCom;
    Safe_AddRef(m_pNoiseTextureCom);

    m_pState = pDesc->pState;
    m_pIsFinished = pDesc->pIsFinished;
    m_pHP = pDesc->pHP;
    m_pDistance = pDesc->pDistance;
    m_pBellyAttackActive = pDesc->pBellyAttackActive;
    m_pMaceAttackActive = pDesc->pMaceAttackActive;
    m_pRatio = pDesc->pRatio;

    m_fSpeed = 2.f;

    return S_OK;
}

_uint CLab_Drum_Body::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CLab_Drum_Body::Update(_float fTimeDelta)
{
    if (CLab_Drum::STATE_IDLE == *m_pState)
        m_eDrumAnim = LAB_DRUM_IDLE;
    else if (CLab_Drum::STATE_SPAWN == *m_pState)
        m_eDrumAnim = LAB_DRUM_SPAWN;
    else if (CLab_Drum::STATE_SUMMON == *m_pState)
        m_eDrumAnim = LAB_DRUM_ATK_DRUM_F_HEAVY;
    else if (CLab_Drum::STATE_DEATH == *m_pState)
        m_eDrumAnim = LAB_DRUM_IMPACT_DEATH;
    else if (CLab_Drum::STATE_ATTACK == *m_pState)
    {
        if (false == m_bAnimStart)
        {
            if (*m_pDistance < 4.f)
                m_eDrumAnim = LAB_DRUM_ATK_BELLY;
            else if (*m_pDistance < 6.f)
                m_eDrumAnim = LAB_DRUM_ATK_SWING_01;

            m_bAnimStart = true;
        }
        _int iFrameIndex = m_pModelCom->Get_KeyFrameIndex();
        if (LAB_DRUM_ATK_BELLY == m_eDrumAnim )
        {
            if(16 <= iFrameIndex && iFrameIndex <= 25)
                *m_pBellyAttackActive = true;
            else
                *m_pBellyAttackActive = false;
        }

        if (LAB_DRUM_ATK_SWING_01 == m_eDrumAnim )
        {
            if (16 <= iFrameIndex && iFrameIndex <= 25)
                *m_pMaceAttackActive = true;
            else
                *m_pMaceAttackActive = false;
        }
    }
    else if (CLab_Drum::STATE_WALK == *m_pState)
    {
        if (*m_pDistance < 4.f)
        {
            m_eDrumAnim = LAB_DRUM_WALK_B;
            m_pConstruct_TransformCom->Go_Backward(fTimeDelta * m_fSpeed, m_pNavigationCom);
        }
        else if (*m_pDistance < 6.f)
        {
            if (LAB_DRUM_WALK_L != m_eDrumAnim && LAB_DRUM_WALK_R != m_eDrumAnim)
            {
                if (m_pGameInstance->Get_Random_Normal() < 0.5f)
                    m_eDrumAnim = LAB_DRUM_WALK_L;
                else
                    m_eDrumAnim = LAB_DRUM_WALK_R;
            }
            if (LAB_DRUM_WALK_L == m_eDrumAnim)
                m_pConstruct_TransformCom->Go_Left(fTimeDelta * m_fSpeed, m_pNavigationCom);
            else if (LAB_DRUM_WALK_R == m_eDrumAnim)
                m_pConstruct_TransformCom->Go_Right(fTimeDelta * m_fSpeed, m_pNavigationCom);
        }
        else
        {
            m_eDrumAnim = LAB_DRUM_WALK_F;
            m_pConstruct_TransformCom->Go_Straight(fTimeDelta * m_fSpeed, m_pNavigationCom);
        }
    }
    else if (CLab_Drum::STATE_IMPACT == *m_pState)
    {
        if (90 > abs(m_fHittedAngle))
            m_eDrumAnim = LAB_DRUM_IMPACT_HEAVY_F;
        else
            m_eDrumAnim = LAB_DRUM_IMPACT_B;
    }

    m_pModelCom->SetUp_Animation(m_eDrumAnim, Animation_Loop(), Animation_NonInterpolate());
    Play_Animation(fTimeDelta);

    if (true == *m_pIsFinished)
        m_bAnimStart = false;
}

void CLab_Drum_Body::Late_Update(_float fTimeDelta)
{
    XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
    m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CLab_Drum_Body::Render()
{
    if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", m_pRatio, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pNoiseTextureCom->Bind_ShadeResource(m_pShaderCom, "g_NoiseTexture", 3)))
        return E_FAIL;

    _float4 vColor = { 1.f, 1.f, 1.f, 1.f };
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vEmissiveColor", &vColor, sizeof(_float4))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ComboTexture", aiTextureType_COMBO, i)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, i)))
            return E_FAIL;


        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLab_Drum_Body::Render_LightDepth()
{
    if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_SHADOWVIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;

}

void CLab_Drum_Body::Reset_Animation()
{
    m_pModelCom->Reset_Animation();
}

_uint CLab_Drum_Body::Get_FrameIndex()
{
    return m_pModelCom->Get_KeyFrameIndex();
}

const _float4x4* CLab_Drum_Body::Get_BoneMatrix_Ptr(const _char* pBoneName) const
{
    return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(pBoneName);
}

_bool CLab_Drum_Body::Animation_Loop()
{
    if (LAB_DRUM_IDLE == m_eDrumAnim
        || LAB_DRUM_WALK_F == m_eDrumAnim
        || LAB_DRUM_WALK_B == m_eDrumAnim
        || LAB_DRUM_WALK_L == m_eDrumAnim
        || LAB_DRUM_WALK_R == m_eDrumAnim)
        return true;

    return false;
}

_bool CLab_Drum_Body::Animation_NonInterpolate()
{
    if (LAB_DRUM_IMPACT_B == m_eDrumAnim ||
        LAB_DRUM_IMPACT_DEATH == m_eDrumAnim ||
        LAB_DRUM_IMPACT_HEAVY_F == m_eDrumAnim||
        LAB_DRUM_SPAWN == m_eDrumAnim)
        return true;

    return false;
}

HRESULT CLab_Drum_Body::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CModel::MODEL_DESC desc = {};
    desc.m_iStartAnim = LAB_DRUM_SPAWN;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Lab_Drum"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &desc)))
        return E_FAIL;

    return S_OK;
}

void CLab_Drum_Body::Play_Animation(_float fTimeDelta)
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

CLab_Drum_Body* CLab_Drum_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLab_Drum_Body* pInstance = new CLab_Drum_Body(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CLab_Drum_Body"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLab_Drum_Body::Clone(void* pArg)
{
    CLab_Drum_Body* pInstance = new CLab_Drum_Body(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CLab_Drum_Body"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CLab_Drum_Body::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pConstruct_TransformCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pNoiseTextureCom);
}
