#include "stdafx.h"
#include "Teleport_Container.h"
#include "GameInstance.h"

#include "Teleport.h"


CTeleport_Container::CTeleport_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject(pDevice, pContext)
{
}

CTeleport_Container::CTeleport_Container(const CTeleport_Container& Prototype)
    : CContainerObject(Prototype)
{
}

HRESULT CTeleport_Container::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTeleport_Container::Initialize(void* pArg)
{

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_PartObject()))
        return E_FAIL;

    return S_OK;
}

_uint CTeleport_Container::Priority_Update(_float fTimeDelta)
{
    for (auto& Part : m_Parts)
    {
        if (nullptr == Part)
            continue;
        Part->Priority_Update(fTimeDelta);
    }

    return OBJ_NOEVENT;
}

void CTeleport_Container::Update(_float fTimeDelta)
{
    for (auto& Part : m_Parts)
    {
        if (nullptr == Part)
            continue;
        Part->Update(fTimeDelta);
    }
}

void CTeleport_Container::Late_Update(_float fTimeDelta)
{
    for (auto& Part : m_Parts)
    {
        if (nullptr == Part)
            continue;
        Part->Late_Update(fTimeDelta);
    }
}

HRESULT CTeleport_Container::Render()
{
    return S_OK;
}

HRESULT CTeleport_Container::Ready_PartObject()
{
    // 경로로 파일 읽어서 어쩌구 저쩌구..strTeleportPath
    _char MaterialFilePath[MAX_PATH]{ "../Bin/SaveData/Teleport.dat" };
    ifstream infile(MaterialFilePath, ios::binary);

    if (!infile.is_open())
        return E_FAIL;

    _uint iPartID = { 0 };
    _uint iIndex = { 0 };

    while (true)
    {
        _float3 vPos = {};
        infile.read(reinterpret_cast<_char*>(&vPos), sizeof(_float3));

        if (true == infile.eof())
            break;

        CTeleport::TELEPORT_DESC desc;
        desc.fRotationPerSec = 0.f;
        desc.fSpeedPerSec = 1.f;
        desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
        desc.vPos = vPos;
        desc.pTeleport = nullptr;

        if (iIndex < 2)
        {
            if (1 == iIndex)
                desc.bActive = true;
            else
                desc.bActive = false;
            desc.vColor = _float4(0.541f, 0.169f, 0.886f, 1.f);
        }
        else if (iIndex < 4)
        {
            if (3 == iIndex)
                desc.bActive = true;
            else
                desc.bActive = false;
            desc.vColor = _float4(1.f, 1.f, 0.4f, 1.f);
        }
        else if (iIndex < 6)
        {
            if (5 == iIndex)
                desc.bActive = true;
            else
                desc.bActive = false;
            desc.vColor = _float4(0.863f, 0.078f, 0.235f, 1.f);
        }
        else
        {
            desc.vColor = _float4(0.541f, 0.169f, 0.886f, 1.f);
            desc.bActive = false;
        }

        CGameObject* pPartObject = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Teleport"), &desc);

        if (nullptr == pPartObject)
            return E_FAIL;

        m_Parts.emplace_back(static_cast<CPartObject*>(pPartObject));

        if (1 == iIndex % 2)
        {
            static_cast<CTeleport*>(m_Parts[iIndex])->Set_Teleport(static_cast<CTeleport*>(m_Parts[iIndex - 1]));
            static_cast<CTeleport*>(m_Parts[iIndex - 1])->Set_Teleport(static_cast<CTeleport*>(m_Parts[iIndex]));
        }

        ++iIndex;
    }

    infile.close();

    return S_OK;
}

CTeleport_Container* CTeleport_Container::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTeleport_Container* pInstance = new CTeleport_Container(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Create Failed : CTeleport_Container"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CTeleport_Container::Clone(void* pArg)
{
    CTeleport_Container* pInstance = new CTeleport_Container(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Create Failed : CTeleport_Container"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CTeleport_Container::Pooling()
{
    return new CTeleport_Container(*this);
}

void CTeleport_Container::Free()
{
    __super::Free();
}
