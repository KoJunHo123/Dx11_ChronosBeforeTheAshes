#include "stdafx.h"
#include "Player.h"
#include "GameInstance.h"


CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    PLAYER_DESC* Desc = static_cast<PLAYER_DESC*>(pArg);

    /* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&Desc->vPos), 1.f));
    m_pTransformCom->Set_Scaled(Desc->vScale.x, Desc->vScale.y, Desc->vScale.z);
    m_pTransformCom->Rotation(XMConvertToRadians(Desc->vRotation.x), XMConvertToRadians(Desc->vRotation.y), XMConvertToRadians(Desc->vRotation.z));
   
    m_Desc = *Desc;

    if (FAILED(Ready_Components()))
        return E_FAIL;
    m_iCurrentAnimationIndex = 49;
    m_pModelCom->SetUp_Animation(m_iCurrentAnimationIndex, false);

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(VK_RIGHT))
        m_pModelCom->SetUp_Animation(++m_iCurrentAnimationIndex, false);

    if (m_pGameInstance->Key_Down(VK_LEFT))
    {
        if (0 < m_iCurrentAnimationIndex)
            m_pModelCom->SetUp_Animation(--m_iCurrentAnimationIndex, false);
    }

}

void CPlayer::Update(_float fTimeDelta)
{
    _vector vStateChange{};
    m_pModelCom->Play_Animation(fTimeDelta, vStateChange);

    vStateChange = Get_Rotation(m_pTransformCom->Get_WorldMatrix(), vStateChange);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vStateChange);
}

void CPlayer::Late_Update(_float fTimeDelta)
{	
    /* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
    __super::Late_Update(fTimeDelta);
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPlayer::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bine_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CPlayer::Save_Data(ofstream* pOutFile)
{
    pOutFile->write(reinterpret_cast<const _char*>(&m_Desc), sizeof(PLAYER_DESC));

    return S_OK;
}


HRESULT CPlayer::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

_vector CPlayer::Get_Rotation(_matrix WorldMatrix, _vector vExist)
{
    _vector vScale, vRotationQuat, vTranslation;
    XMMatrixDecompose(&vScale, &vRotationQuat, &vTranslation, WorldMatrix);

    // 1번 방법
    // 회전 행렬 생성 (쿼터니언을 회전 행렬로 변환)
    XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(vRotationQuat);
    // 회전 행렬을 벡터 v에 적용
    return XMVector3TransformNormal(vExist, rotationMatrix);

    // 2번 방법 : 이거 뭔가 이상하게 됨.
    //// 쿼터니언의 역수 계산
    //_vector qInverse = XMQuaternionInverse(vRotationQuat);

    //// v를 쿼터니언으로 변환 (w=0, x, y, z = 벡터 값)
    //_vector vQuat = XMVectorSet(XMVectorGetX(vExist), XMVectorGetY(vExist), XMVectorGetZ(vExist), 0.0f);

    //// 회전 적용: v' = q * v * q^-1
    //_vector rotatedVQuat = XMQuaternionMultiply(XMQuaternionMultiply(vRotationQuat, vQuat), qInverse);

    //// 회전된 벡터 추출 (x, y, z)
    //return XMVectorSet(XMVectorGetX(rotatedVQuat), XMVectorGetY(rotatedVQuat), XMVectorGetZ(rotatedVQuat), 0.0f);
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
