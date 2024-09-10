#include "stdafx.h"
#include "Lab_Mage_Body.h"
#include "GameInstance.h"

#include "Lab_Mage.h"

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

    m_pMage_TransformCom = pDesc->pMage_TransformCom;
    Safe_AddRef(m_pMage_TransformCom);

    m_pPlayer_TransformCom = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), g_strTransformTag));
    Safe_AddRef(m_pPlayer_TransformCom);

    m_pState = pDesc->pState;
    m_pIsFinished = pDesc->pIsFinished;
    m_pHP = pDesc->pHP;
    m_pDistance = pDesc->pDistance;
    m_pAnimStart = pDesc->pAnimStart;
    m_pAnimOver = pDesc->pAnimOver;
    m_pAttackActive = pDesc->pAttackActive;

    m_fSpeed = 3.f;

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
    if (CLab_Mage::STATE_IDLE == *m_pState)
        m_eMageAnim = LAB_MAGE_IDLE;
    else if (CLab_Mage::STATE_DASH_B == *m_pState)
    {
        m_eMageAnim = LAB_MAGE_DASH_B_START;
        if (true == *m_pIsFinished)
        {
            *m_pAnimOver = true;
        }
    }
    else if (CLab_Mage::STATE_SPAWN == *m_pState)
    {
        m_eMageAnim = LAB_MAGE_MIRROR;
        if (true == *m_pIsFinished)
        {
            *m_pAnimOver = true;
        }
    }
    else if (CLab_Mage::STATE_ATTACK_COMBO == *m_pState)
    {
        if (false == *m_pAnimStart)
        {
            m_eMageAnim = LAB_MAGE_ATK_COMBO_01_START;
            *m_pAnimStart = true;
        }

        if (true == *m_pIsFinished)
        {
            if (LAB_MAGE_ATK_COMBO_01_START == m_eMageAnim)
            {
                if (*m_pDistance > 5.f)
                {
                    // 텔포.
                    m_pMage_TransformCom->Set_State(CTransform::STATE_POSITION, Find_TeleportPos());
                    m_pMage_TransformCom->LookAt(m_pPlayer_TransformCom->Get_State(CTransform::STATE_POSITION));
                }
                m_eMageAnim = LAB_MAGE_ATK_COMBO_01_STRIKE;
            }
            else if (LAB_MAGE_ATK_COMBO_01_STRIKE == m_eMageAnim)
            {
                if(*m_pDistance > 5.f)
                    m_eMageAnim = LAB_MAGE_ATK_COMBO_02_START;
                else
                {
                    *m_pAnimStart = false;
                    *m_pAnimOver = true;
                }
            }
            else if (LAB_MAGE_ATK_COMBO_02_START == m_eMageAnim)
            {
                if (*m_pDistance > 5.f)
                {
                    // 텔포.
                    m_pMage_TransformCom->Set_State(CTransform::STATE_POSITION, Find_TeleportPos());
                    m_pMage_TransformCom->LookAt(m_pPlayer_TransformCom->Get_State(CTransform::STATE_POSITION));
                }

                m_eMageAnim = LAB_MAGE_ATK_COMBO_02_STRIKE;
            }
            else if (LAB_MAGE_ATK_COMBO_02_STRIKE == m_eMageAnim)
            {
                *m_pAnimStart = false;
                *m_pAnimOver = true;
            }
        }

        _uint iFrameIndex = m_pModelCom->Get_KeyFrameIndex();
        if (LAB_MAGE_ATK_COMBO_01_STRIKE == m_eMageAnim)
        {
            if (8 <= iFrameIndex && iFrameIndex <= 14)
                *m_pAttackActive = true;
            else if (31 <= iFrameIndex && iFrameIndex <= 36)
                *m_pAttackActive = true;
            else
                *m_pAttackActive = false;
        }
        else if(LAB_MAGE_ATK_COMBO_02_STRIKE == m_eMageAnim)
        {
            if (3 <= iFrameIndex && iFrameIndex <= 8)
                *m_pAttackActive = true;
            else if (23 <= iFrameIndex && iFrameIndex <= 30)
                *m_pAttackActive = true;
            else
                *m_pAttackActive = false;
        }
    }
    else if (CLab_Mage::STATE_ATTACK_SLASH == *m_pState)
    {
        if (false == *m_pAnimStart)
        {
            m_eMageAnim = LAB_MAGE_ATK_SLASH_01_START;
            *m_pAnimStart = true;
        }

        if (true == *m_pIsFinished)
        {
            if (LAB_MAGE_ATK_SLASH_01_START == m_eMageAnim)
            {
                if (*m_pDistance > 5.f)
                {
                    // 텔포.
                    m_pMage_TransformCom->Set_State(CTransform::STATE_POSITION, Find_TeleportPos());
                    m_pMage_TransformCom->LookAt(m_pPlayer_TransformCom->Get_State(CTransform::STATE_POSITION));
                }

                m_eMageAnim = LAB_MAGE_ATK_SLASH_01_STRIKE;
            }
            else if (LAB_MAGE_ATK_SLASH_01_STRIKE == m_eMageAnim)
            {
                if (*m_pDistance > 5.f)
                    m_eMageAnim = LAB_MAGE_ATK_SLASH_02_START;
                else
                {
                    *m_pAnimStart = false;
                    *m_pAnimOver = true;
                }
            }
            else if (LAB_MAGE_ATK_SLASH_02_START == m_eMageAnim)
            {
                if (*m_pDistance > 5.f)
                {
                    // 텔포.
                    m_pMage_TransformCom->Set_State(CTransform::STATE_POSITION, Find_TeleportPos());
                    m_pMage_TransformCom->LookAt(m_pPlayer_TransformCom->Get_State(CTransform::STATE_POSITION));
                }

                m_eMageAnim = LAB_MAGE_ATK_SLASH_02_STRIKE;
            }
            else if (LAB_MAGE_ATK_SLASH_02_STRIKE == m_eMageAnim)
            {
                *m_pAnimStart = false;
                *m_pAnimOver = true;
            }
        }

        _uint iFrameIndex = m_pModelCom->Get_KeyFrameIndex();
        if (LAB_MAGE_ATK_SLASH_01_STRIKE == m_eMageAnim && 4 <= iFrameIndex && iFrameIndex <= 8)
            *m_pAttackActive = true;
        else if (LAB_MAGE_ATK_SLASH_02_STRIKE == m_eMageAnim && 4 <= iFrameIndex && iFrameIndex <= 10)
            *m_pAttackActive = true;
        else
            *m_pAttackActive = false;

    }
    else if (CLab_Mage::STATE_WALK == *m_pState)
    {
        if (*m_pDistance < 5.f)
        {
            m_eMageAnim = LAB_MAGE_WALK_B;
            m_pMage_TransformCom->Go_Backward(fTimeDelta * m_fSpeed, m_pNavigationCom);
        }
        else if (*m_pDistance < 10.f)
        {
            if (LAB_MAGE_WALK_L != m_eMageAnim && LAB_MAGE_WALK_R != m_eMageAnim)
            {
                if (m_pGameInstance->Get_Random_Normal() < 0.5f)
                    m_eMageAnim = LAB_MAGE_WALK_L;
                else
                    m_eMageAnim = LAB_MAGE_WALK_R;
            }
            if (LAB_MAGE_WALK_L == m_eMageAnim)
                m_pMage_TransformCom->Go_Left(fTimeDelta * m_fSpeed, m_pNavigationCom);
            else if (LAB_MAGE_WALK_R == m_eMageAnim)
                m_pMage_TransformCom->Go_Right(fTimeDelta * m_fSpeed, m_pNavigationCom);
        }
        else
        {
            m_eMageAnim = LAB_MAGE_WALK_F;
            m_pMage_TransformCom->Go_Straight(fTimeDelta * m_fSpeed, m_pNavigationCom);
        }
    }
    else if (CLab_Mage::STATE_IMPACT == *m_pState)
    {
        if (90 > abs(m_fHittedAngle))
        {
            if(15 < m_iImpactedDamage)
                m_eMageAnim = LAB_MAGE_IMPACT_HEAVY_F;
            else
            {
                if(LAB_MAGE_IMPACT_FL != m_eMageAnim && LAB_MAGE_IMPACT_FR != m_eMageAnim)
                {
                    if (0.5 < m_pGameInstance->Get_Random_Normal())
                        m_eMageAnim = LAB_MAGE_IMPACT_FL;
                    else
                        m_eMageAnim = LAB_MAGE_IMPACT_FR;
                }
            }
            
        }
        else
            m_eMageAnim = LAB_MAGE_IMPACT_B;

        if (true == *m_pIsFinished)
        {
            *m_pAnimOver = true;
            *m_pAnimStart = false;
        }

    }
    else if(CLab_Mage::STATE_DEATH == *m_pState)
        m_eMageAnim = LAB_MAGE_IMPACT_DEATH;

    m_pModelCom->SetUp_Animation(m_eMageAnim, Animation_Loop(), Animation_NonInterpolate());
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

void CLab_Mage_Body::Reset_Animation()
{
    m_pModelCom->Reset_Animation();
}

_uint CLab_Mage_Body::Get_FrameIndex()
{
    return m_pModelCom->Get_KeyFrameIndex();
}

const _float4x4* CLab_Mage_Body::Get_BoneMatrix_Ptr(const _char* pBoneName) const
{
    return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(pBoneName);
}

_bool CLab_Mage_Body::Animation_Loop()
{
    if (LAB_MAGE_IDLE == m_eMageAnim
        || LAB_MAGE_WALK_F == m_eMageAnim
        || LAB_MAGE_WALK_B == m_eMageAnim
        || LAB_MAGE_WALK_L == m_eMageAnim
        || LAB_MAGE_WALK_R == m_eMageAnim)
        return true;

    return false;
}

_bool CLab_Mage_Body::Animation_NonInterpolate()
{
    if (LAB_MAGE_IMPACT_B == m_eMageAnim ||
        LAB_MAGE_IMPACT_DEATH == m_eMageAnim ||
        LAB_MAGE_IMPACT_HEAVY_F == m_eMageAnim ||
        LAB_MAGE_IMPACT_FL == m_eMageAnim ||
        LAB_MAGE_IMPACT_FR == m_eMageAnim ||
        LAB_MAGE_MIRROR == m_eMageAnim)
        return true;

    return false;
}

_vector CLab_Mage_Body::Find_TeleportPos()
{
    _vector vDir = m_pMage_TransformCom->Get_State(CTransform::STATE_POSITION) - m_pPlayer_TransformCom->Get_State(CTransform::STATE_POSITION);
    vDir = XMVector3Normalize(vDir);

    _vector vTeleportPos = m_pPlayer_TransformCom->Get_State(CTransform::STATE_POSITION) + vDir * 2.5f;

    _vector vDummy = {};
    if (true == m_pNavigationCom ->isMove(vTeleportPos, &vDummy))
        return vTeleportPos;
        
    return m_pMage_TransformCom->Get_State(CTransform::STATE_POSITION);
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

    vStateChange = m_pMage_TransformCom->Get_Rotated_Vector(vStateChange);

    _vector vBossPos = m_pMage_TransformCom->Get_State(CTransform::STATE_POSITION);

    _vector vMovePosition = vBossPos + vStateChange;

    _vector vLine = {};

    if (nullptr == m_pNavigationCom || true == m_pNavigationCom->isMove(vMovePosition, &vLine))
        m_pMage_TransformCom->Set_State(CTransform::STATE_POSITION, vMovePosition);
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
            m_pMage_TransformCom->Set_State(CTransform::STATE_POSITION, vMovePosition);
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
    Safe_Release(m_pMage_TransformCom);
    Safe_Release(m_pPlayer_TransformCom);

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
