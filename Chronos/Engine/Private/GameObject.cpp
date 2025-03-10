#include "..\Public\GameObject.h"
#include "GameInstance.h"


CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance() } 
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & Prototype)
	: m_pDevice { Prototype.m_pDevice }
	, m_pContext { Prototype.m_pContext }
	, m_pGameInstance{ Prototype.m_pGameInstance }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	m_isCloned = true;
}

_vector CGameObject::Get_State(_uint iState)
{
	if (iState >= CTransform::STATE_END)
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);

	return m_pTransformCom->Get_State(CTransform::STATE(iState));
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	GAMEOBJECT_DESC* pDesc = nullptr != pArg ? static_cast<GAMEOBJECT_DESC*>(pArg) : nullptr;

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext, pDesc);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	/* �ٸ� ��ü�� �� ������Ʈ�� �˻��� �� �ֵ��� �ʿ��ٰ� ������Ʈ�� ��Ƶд�. */
	m_Components.emplace(g_strTransformTag, m_pTransformCom);

	Safe_AddRef(m_pTransformCom);

	return S_OK;
}

_uint CGameObject::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CGameObject::Update(_float fTimeDelta)
{
}

void CGameObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

CComponent * CGameObject::Find_Component(const _wstring & strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

CComponent* CGameObject::Find_PartComponent(const _wstring& strComponentTag, _uint iPartObjIndex)
{
	return nullptr;
}

void CGameObject::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
}

_vector CGameObject::Get_Position()
{
	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

void CGameObject::Set_Position(_vector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const _wstring & strPrototypeTag, const _wstring & strComponentTag, CComponent** ppOut, void * pArg)
{
	if (nullptr != Find_Component(strComponentTag))
		return E_FAIL;

	CComponent*		pComponent = m_pGameInstance->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);
	
	return S_OK;
}

void CGameObject::Free()
{
	__super::Free();

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pTransformCom);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

void CGameObject::Return()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	Safe_Release(m_pTransformCom);

	m_Components.clear();

	m_bDead = false;
}
