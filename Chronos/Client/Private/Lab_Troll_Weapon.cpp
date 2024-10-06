#include "stdafx.h"
#include "Lab_Troll_Weapon.h"
#include "GameInstance.h"

#include "Player.h"
#include "Effect_BloodCore.h"

CLab_Troll_Weapon::CLab_Troll_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject(pDevice, pContext)
{
}

CLab_Troll_Weapon::CLab_Troll_Weapon(const CLab_Troll_Weapon& Prototype)
    : CPartObject(Prototype)
{
}

HRESULT CLab_Troll_Weapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLab_Troll_Weapon::Initialize(void* pArg)
{
    WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);
    
    m_pNoiseTextureCom = pDesc->pNoiseTextureCom;
    Safe_AddRef(m_pNoiseTextureCom);


    if(FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_fDamage = pDesc->fDamage;
    m_pAttackActive = pDesc->pAttackActive;
    m_pSocketMatrix = pDesc->pSocketBoneMatrix;

    m_pRatio = pDesc->pRatio;
    m_vCenter = pDesc->vCenter;

    if (FAILED(Ready_Components(pDesc->vExtents, pDesc->vCenter, pDesc->vAngles)))
        return E_FAIL;

    m_pTransformCom->Rotation(-90.f, 0.f, 0.f);

    return S_OK;
}

_uint CLab_Troll_Weapon::Priority_Update(_float fTimeDelta)
{
    m_pColliderCom->Set_OnCollision(*m_pAttackActive);

    XMStoreFloat3(&m_vPrePosition, XMLoadFloat4x4(&m_WorldMatrix).r[3]);

    return OBJ_NOEVENT;
}

void CLab_Troll_Weapon::Update(_float fTimeDelta)
{
    _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    // ??
    for (size_t i = 0; i < 3; i++)
    {
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
    }

    // 내 위치 * 붙여야 할 뼈의 위치 * 플레이어 위치 -> 플레이어의 위치에서 붙여야 할 뼈의 위치.
    // -> 셰이더에서 해주던 뼈 * 월드를 여기서 하는 거.
    XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
    m_pColliderCom->Update(&m_WorldMatrix);

}

void CLab_Troll_Weapon::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
#ifdef _DEBUG
    m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif

}

HRESULT CLab_Troll_Weapon::Render()
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
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ComboTexture", aiTextureType_COMBO, i)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }


    return S_OK;
}

void CLab_Troll_Weapon::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
    if (true == *m_pAttackActive && TEXT("Coll_Player") == strColliderTag)
    {
        CPlayer* pPlayer = static_cast<CPlayer*>(pCollisionObject);
        if (true == pPlayer->Be_Damaged(m_fDamage, XMLoadFloat4x4(m_pParentMatrix).r[3]))
        {
            _vector vCenter = XMLoadFloat3(&m_vCenter);
            vCenter = XMVector3TransformCoord(vCenter, XMLoadFloat4x4(&m_WorldMatrix));
            _vector vTargetPos = pCollisionObject->Get_Position();
            vTargetPos.m128_f32[1] = vCenter.m128_f32[1];
            _vector vDir = vTargetPos - vCenter;
            _vector vPos = vCenter + vDir * 0.5f;
            _vector vMoveDir = XMLoadFloat4x4(&m_WorldMatrix).r[3] - XMLoadFloat3(&m_vPrePosition);
            Add_AttackParticle(vPos, XMVector3Normalize(vMoveDir));
        }
    }
}

HRESULT CLab_Troll_Weapon::Ready_Components(_float3 vExtents, _float3 vCenter, _float3 vAngles)
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Lab_Troll_Knife"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC			ColliderOBBDesc{};
    ColliderOBBDesc.vExtents = vExtents;
    ColliderOBBDesc.vCenter = vCenter;
    ColliderOBBDesc.vAngles = vAngles;

    CCollider::COLLIDER_DESC ColliderDesc = {};
    ColliderDesc.pOwnerObject = this;
    ColliderDesc.pBoundingDesc = &ColliderOBBDesc;
    ColliderDesc.bCollisionOnce = true;
    ColliderDesc.strColliderTag = TEXT("Coll_Monster_Attack");

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLab_Troll_Weapon::Add_AttackParticle(_fvector vPos, _fvector vDir)
{
    CEffect_BloodCore::BLOOD_DESC desc = {};

    desc.fRotationPerSec = 0.f;
    desc.fSpeedPerSec = 1.f;
    XMStoreFloat3(&desc.vPos, vPos);
    XMStoreFloat3(&desc.vDir, vDir);
    desc.vScale = _float3(5.f, 5.f, 5.f);

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_BloodCore"), &desc)))
        return E_FAIL;

    return S_OK;
}

CLab_Troll_Weapon* CLab_Troll_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLab_Troll_Weapon* pInstance = new CLab_Troll_Weapon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CLab_Troll_Weapon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CLab_Troll_Weapon* CLab_Troll_Weapon::Clone(void* pArg)
{
    CLab_Troll_Weapon* pInstance = new CLab_Troll_Weapon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Clone Failed : CLab_Troll_Weapon"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLab_Troll_Weapon::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pNoiseTextureCom);
}
