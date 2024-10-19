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

    m_eConstructAnim = LAB_CONSTRUCT_IDLE;

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

    m_pSwordAttackActive = pDesc->pSwordAttackActive;
    m_pShieldAttackActive = pDesc->pShieldAttackActive;
    m_pRatio = pDesc->pRatio;
    m_pCurrentAnim = pDesc->pCurrentAnim;

    m_fSpeed = 2.5f;

    return S_OK;
}

_uint CLab_Construct_Body::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    *m_pCurrentAnim = m_eConstructAnim;

    return OBJ_NOEVENT;
}

void CLab_Construct_Body::Update(_float fTimeDelta)
{
    if (CLab_Construct::STATE_IDLE == *m_pState)
    {
        m_eConstructAnim = LAB_CONSTRUCT_IDLE;
    }
    else if (CLab_Construct::STATE_DEATH == *m_pState)
        m_eConstructAnim = LAB_CONSTRUCT_IMPACT_DEATH;
    else if (CLab_Construct::STATE_ATTACK == *m_pState)
    {
        if (false == m_bAnimStart)
        {
            if (*m_pDistance < 5.f)
                m_eConstructAnim = LAB_CONSTRUCT_ATK_SHIELDSWIPE;
            else if (*m_pDistance < 8.f)
                m_eConstructAnim = LAB_CONSTRUCT_ATK_DOWNSWIPE;
            else if (*m_pDistance < 12.f)
                m_eConstructAnim = LAB_CONSTRUCT_ATK_VERTICALSLAM;
            else if (*m_pDistance < 15.f)
            {
                m_eConstructAnim = LAB_CONSTRUCT_ATK_3HIT_COMBO;

                SOUND_DESC desc = {};
                desc.fMaxDistance = DEFAULT_DISTANCE;
                desc.fVolume = 1.f;
                XMStoreFloat3(&desc.vPos, m_pConstruct_TransformCom->Get_State(CTransform::STATE_POSITION));

                m_pGameInstance->SoundPlay_Additional(TEXT("SFX_Lab_Construct_Powerup_02.ogg"), desc);
            }

            m_bAnimStart = true;
        }

        _uint iFrameIndex = m_pModelCom->Get_KeyFrameIndex();
        if (LAB_CONSTRUCT_ATK_SHIELDSWIPE == m_eConstructAnim && 21 <= iFrameIndex && iFrameIndex <= 30)
            *m_pShieldAttackActive = true;
        else
            *m_pShieldAttackActive = false;

        if (LAB_CONSTRUCT_ATK_DOWNSWIPE == m_eConstructAnim && 28 <= iFrameIndex && iFrameIndex <= 37)
            *m_pSwordAttackActive = true;
        else if (LAB_CONSTRUCT_ATK_VERTICALSLAM == m_eConstructAnim && 25 <= iFrameIndex && iFrameIndex <= 32)
            *m_pSwordAttackActive = true;
        else if (LAB_CONSTRUCT_ATK_3HIT_COMBO == m_eConstructAnim && ((58 <= iFrameIndex && iFrameIndex <= 67)
            || (123 <= iFrameIndex && iFrameIndex <= 132) || (185 <= iFrameIndex && iFrameIndex <= 200)))
            *m_pSwordAttackActive = true;
        else
            *m_pSwordAttackActive = false;
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
            if (LAB_CONSTRUCT_WALK_L != m_eConstructAnim && LAB_CONSTRUCT_WALK_R != m_eConstructAnim)
            {
                if (m_pGameInstance->Get_Random_Normal() < 0.5f)
                    m_eConstructAnim = LAB_CONSTRUCT_WALK_L;
                else
                    m_eConstructAnim = LAB_CONSTRUCT_WALK_R;
            }
            if (LAB_CONSTRUCT_WALK_L == m_eConstructAnim)
                m_pConstruct_TransformCom->Go_Left(fTimeDelta * m_fSpeed, m_pNavigationCom);
            else if (LAB_CONSTRUCT_WALK_R == m_eConstructAnim)
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
    Play_Animation(fTimeDelta );


    if (true == *m_pIsFinished)
        m_bAnimStart = false;
}

void CLab_Construct_Body::Late_Update(_float fTimeDelta)
{
    XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

    StepSound();

    if (true == m_pGameInstance->isIn_Frustum_WorldSpace(XMLoadFloat4x4(&m_WorldMatrix).r[3], 5.f))
    {
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
        m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
    }
}

HRESULT CLab_Construct_Body::Render()
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

HRESULT CLab_Construct_Body::Render_LightDepth()
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

void CLab_Construct_Body::Reset_Animation()
{
    m_pModelCom->Reset_Animation();
}

_uint CLab_Construct_Body::Get_FrameIndex()
{
    return m_pModelCom->Get_KeyFrameIndex();
}

const _float4x4* CLab_Construct_Body::Get_BoneMatrix_Ptr(const _char* pBoneName) const
{
    return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(pBoneName);
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

void CLab_Construct_Body::StepSound()
{
    if(CLab_Construct::STATE_WALK == *m_pState)
    {
        _uint iFrameIndex = Get_FrameIndex();

        if (false == m_bLeftStep && 27 <= iFrameIndex && iFrameIndex < 66)
        {
            SOUND_DESC desc = {};

            desc.fMaxDistance = DEFAULT_DISTANCE;
            XMStoreFloat3(& desc.vPos, m_pConstruct_TransformCom->Get_State(CTransform::STATE_POSITION));
            desc.fVolume = 0.25f;

            m_pGameInstance->SoundPlay_Additional(TEXT("Construct_FS_Stone_Low_01.ogg"), desc);
            m_bLeftStep = true;
            m_bRightStep = false;
        }
        else if (false == m_bRightStep && 66 <= iFrameIndex)
        {
            SOUND_DESC desc = {};

            desc.fMaxDistance = DEFAULT_DISTANCE;
            XMStoreFloat3(&desc.vPos, m_pConstruct_TransformCom->Get_State(CTransform::STATE_POSITION));
            desc.fVolume = 0.25f;

            m_pGameInstance->SoundPlay_Additional(TEXT("Construct_FS_Stone_Low_02.ogg"), desc);
            m_bLeftStep = false;
            m_bRightStep = true;
        }
    }
    else
    {
        m_bLeftStep = false;
        m_bRightStep = false;
    }

}

HRESULT CLab_Construct_Body::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    CModel::MODEL_DESC desc = {};
    desc.m_iStartAnim = m_eConstructAnim;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Lab_Construct"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &desc)))
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
    Safe_Release(m_pNoiseTextureCom);
}
