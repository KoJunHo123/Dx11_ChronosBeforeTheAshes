#include "stdafx.h"
#include "Lab_Troll_Body.h"
#include "GameInstance.h"

#include "Lab_Troll.h"

CLab_Troll_Body::CLab_Troll_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{
}

CLab_Troll_Body::CLab_Troll_Body(const CLab_Troll_Body& Prototype)
    : CPartObject(Prototype)
{
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

    m_pTroll_TransformCom = pDesc->pConstruct_TransformCom;
    Safe_AddRef(m_pTroll_TransformCom);

    m_pState = pDesc->pState;
    m_pIsFinished = pDesc->pIsFinished;
    m_pHP = pDesc->pHP;
    m_pDistance = pDesc->pDistance;
    m_pLeftAttackActive     = pDesc->pLeftAttackActive;
    m_pRightAttackActive    = pDesc->pRightAttackActive;
    m_fSpeed = 5.f;

    return S_OK;
}

_uint CLab_Troll_Body::Priority_Update(_float fTimeDelta)
{
    if (true == m_bDead)
        return OBJ_DEAD;

    return OBJ_NOEVENT;
}

void CLab_Troll_Body::Update(_float fTimeDelta)
{
    if (CLab_Troll::STATE_IDLE == *m_pState)
        m_eTrollAnim = LAB_TROLL_IDLE;
    else if (CLab_Troll::STATE_SPAWN == *m_pState)
        m_eTrollAnim = LAB_TROLL_SPAWN;
    else if (CLab_Troll::STATE_DEATH == *m_pState)
        m_eTrollAnim = LAB_TROLL_IMPACT_DEATH;
    else if (CLab_Troll::STATE_SPRINT == *m_pState)
    {
        m_eTrollAnim = LAB_TROLL_SPRINT_F;
        m_pTroll_TransformCom->Go_Straight(fTimeDelta * m_fSpeed * 2.f, m_pNavigationCom);
    }
    else if (CLab_Troll::STATE_ATTACK == *m_pState)
    {
        if (false == m_bAnimStart)
        {
            _float fRandom = m_pGameInstance->Get_Random_Normal();
            if (*m_pDistance < 3.f)
            {
                m_eTrollAnim = LAB_TROLL_ATK_UPPERCUT;
            }
            else if (*m_pDistance < 7.f)
            {
                if (LAB_TROLL_ATK_SWIPE_L != m_eTrollAnim && LAB_TROLL_ATK_SWIPE_R != m_eTrollAnim)
                {
                    if (fRandom < 0.5f)
                        m_eTrollAnim = LAB_TROLL_ATK_SWIPE_L;
                    else
                        m_eTrollAnim = LAB_TROLL_ATK_SWIPE_R;

                }
            }
            else if (*m_pDistance < 10.f)
            {
                if (LAB_TROLL_ATK_CHARGE_L != m_eTrollAnim && LAB_TROLL_ATK_CHARGE_R != m_eTrollAnim)
                {
                    if (fRandom < 0.5f)
                        m_eTrollAnim = LAB_TROLL_ATK_CHARGE_L;
                    else
                        m_eTrollAnim = LAB_TROLL_ATK_CHARGE_R;

                }
            }
            m_bAnimStart = true;
        }

        _uint iFrameIndex = m_pModelCom->Get_KeyFrameIndex();

        if (LAB_TROLL_ATK_CHARGE_L == m_eTrollAnim && 23 <= iFrameIndex && iFrameIndex <= 28)
            *m_pLeftAttackActive = true;
        else if(LAB_TROLL_ATK_CHARGE_R == m_eTrollAnim && 23 <= iFrameIndex && iFrameIndex <= 28)
            *m_pRightAttackActive = true;
        else if(LAB_TROLL_ATK_SWIPE_L == m_eTrollAnim && 20 <= iFrameIndex && iFrameIndex <=25)
            *m_pLeftAttackActive = true;
        else if (LAB_TROLL_ATK_SWIPE_R == m_eTrollAnim && 20 <= iFrameIndex && iFrameIndex <= 25)
            *m_pRightAttackActive = true;
        else if (LAB_TROLL_ATK_UPPERCUT == m_eTrollAnim && 24 <= iFrameIndex && iFrameIndex <= 30)
        {
            *m_pLeftAttackActive = true;
        }
        else 
        {
            *m_pLeftAttackActive = false;
            *m_pRightAttackActive = false;
        }


    }
    else if (CLab_Troll::STATE_WALK == *m_pState)
    {
        if (*m_pDistance < 5.f)
        {
            m_eTrollAnim = LAB_TROLL_WALK_B;
            m_pTroll_TransformCom->Go_Backward(fTimeDelta * m_fSpeed, m_pNavigationCom);
        }
        else if (*m_pDistance < 10.f)
        {
            if (LAB_TROLL_WALK_L != m_eTrollAnim && LAB_TROLL_WALK_R != m_eTrollAnim)
            {
                if (m_pGameInstance->Get_Random_Normal() < 0.5f)
                    m_eTrollAnim = LAB_TROLL_WALK_L;
                else
                    m_eTrollAnim = LAB_TROLL_WALK_R;
            }
        }
        else
        {
            m_eTrollAnim = LAB_TROLL_WALK_F;
            m_pTroll_TransformCom->Go_Straight(fTimeDelta * m_fSpeed, m_pNavigationCom);
        }

        if (LAB_TROLL_WALK_L == m_eTrollAnim)
            m_pTroll_TransformCom->Go_Left(fTimeDelta * m_fSpeed, m_pNavigationCom);
        else if (LAB_TROLL_WALK_R == m_eTrollAnim)
            m_pTroll_TransformCom->Go_Right(fTimeDelta * m_fSpeed, m_pNavigationCom);

    }
    else if(CLab_Troll::STATE_IMPACT == *m_pState)
    {
        if (90 > abs(m_fHittedAngle))
        {
            {
                if (15 < m_iImpactedDamage)
                    m_eTrollAnim = LAB_TROLL_IMPACT_HEAVY;

                if (0.f <= m_fHittedAngle && m_fHittedAngle < 90.f)
                    m_eTrollAnim = LAB_TROLL_IMPACT_FROML;
                else if (-90.f <= m_fHittedAngle && m_fHittedAngle < 0.f)
                    m_eTrollAnim = LAB_TROLL_IMPACT_FROMR;
            }
        }
        else
            m_eTrollAnim = LAB_TROLL_IMPACT_FROMB;
    }

    m_pModelCom->SetUp_Animation(m_eTrollAnim, Animation_Loop(), Animation_NonInterpolate());
    Play_Animation(fTimeDelta);

    if (true == *m_pIsFinished)
        m_bAnimStart = false;
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

void CLab_Troll_Body::Reset_Animation()
{
    m_pModelCom->Reset_Animation();
}

const _float4x4* CLab_Troll_Body::Get_BoneMatrix_Ptr(const _char* pBoneName) const
{
    return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(pBoneName);
}

_uint CLab_Troll_Body::Get_FrameIndex()
{
    return m_pModelCom->Get_KeyFrameIndex();
}


_bool CLab_Troll_Body::Animation_Loop()
{
    if (LAB_TROLL_IDLE == m_eTrollAnim
        || LAB_TROLL_WALK_F == m_eTrollAnim
        || LAB_TROLL_WALK_B == m_eTrollAnim
        || LAB_TROLL_WALK_L == m_eTrollAnim
        || LAB_TROLL_WALK_R == m_eTrollAnim
        || LAB_TROLL_SPRINT_F == m_eTrollAnim)
        return true;

    return false;
}

_bool CLab_Troll_Body::Animation_NonInterpolate()
{
    if (LAB_TROLL_IMPACT_FROMB == m_eTrollAnim ||
        LAB_TROLL_IMPACT_FROML == m_eTrollAnim ||
        LAB_TROLL_IMPACT_FROMR == m_eTrollAnim ||
        LAB_TROLL_IMPACT_HEAVY == m_eTrollAnim ||
        LAB_TROLL_IMPACT_DEATH == m_eTrollAnim || 
        LAB_TROLL_SPAWN == m_eTrollAnim)
        return true;

    return false;
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

    vStateChange = m_pTroll_TransformCom->Get_Rotated_Vector(vStateChange);

    _vector vBossPos = m_pTroll_TransformCom->Get_State(CTransform::STATE_POSITION);

    _vector vMovePosition = vBossPos + vStateChange;

    _vector vLine = {};

    if (nullptr == m_pNavigationCom || true == m_pNavigationCom->isMove(vMovePosition, &vLine))
        m_pTroll_TransformCom->Set_State(CTransform::STATE_POSITION, vMovePosition);
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
            m_pTroll_TransformCom->Set_State(CTransform::STATE_POSITION, vMovePosition);
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
    Safe_Release(m_pTroll_TransformCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
