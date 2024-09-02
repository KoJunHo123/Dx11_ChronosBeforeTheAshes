#include "stdafx.h"
#include "..\Public\Monster.h"
#include "GameInstance.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject( pDevice, pContext )
{
}

CMonster::CMonster(const CMonster& Prototype)
	: CContainerObject( Prototype )
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	MONSTER_DESC*		pDesc = static_cast<MONSTER_DESC*>(pArg);

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));

	if(FAILED(Ready_Components(pDesc->iStartCellIndex)))
		return E_FAIL;

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{

}

void CMonster::Update(_float fTimeDelta)
{
	m_pTransformCom->SetUp_OnCell(m_pNavigationCom);
}

void CMonster::Late_Update(_float fTimeDelta)
{
}

HRESULT CMonster::Render()
{
	return S_OK;
}

HRESULT CMonster::Ready_Components(_int iStartCellIndex)
{
	CNavigation::NAVIGATION_DESC desc{};
	desc.iCurrentIndex = iStartCellIndex;

	/* FOR.Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &desc)))
		return E_FAIL;

	return S_OK;
}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);

}
