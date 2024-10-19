#include "stdafx.h"
#include "..\Public\Monster.h"
#include "GameInstance.h"

#include "UI_MonsterHPBar.h"

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

	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);

	if(FAILED(Ready_Components(pDesc->iStartCellIndex)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pNavigationCom->Get_CellCenterPos(pDesc->iStartCellIndex));

	m_pPlayerTransformCom = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), g_strTransformTag, 0));
	Safe_AddRef(m_pPlayerTransformCom);

	m_pNavigationCom->Set_SkipTypeIndex(1);

	return S_OK;
}

_uint CMonster::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CMonster::Update(_float fTimeDelta)
{
	m_pTransformCom->SetUp_OnCell(m_pNavigationCom);
	
	if(0.f < m_fDamagedDelay)
		m_fDamagedDelay -= fTimeDelta;
	
	//m_fHittedDelay += fTimeDelta;
}

void CMonster::Late_Update(_float fTimeDelta)
{
}

HRESULT CMonster::Render()
{
	return S_OK;
}

void CMonster::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
	if (TEXT("Coll_Monster") == strColliderTag || TEXT("Coll_Player") == strColliderTag)
	{
		_vector vDir = {};
		_vector vCollisionPos = pCollisionObject->Get_Position();
		vDir = vCollisionPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float fX = abs(vDir.m128_f32[0]);
		_float fY = abs(vDir.m128_f32[1]);
		_float fZ = abs(vDir.m128_f32[2]);

		_float fLength = { 0.f };

		if (fX >= fY && fX >= fZ)
		{
			vDir = XMVectorSet(vDir.m128_f32[0], 0.f, 0.f, 0.f);	// 나가야 되는 방향.
			fLength = vSourInterval.x + vDestInterval.x;	// 전체 거리.
		}
		else if (fY >= fX && fY >= fZ)
		{
			vDir = XMVectorSet(0.f, vDir.m128_f32[1], 0.f, 0.f);
			fLength = vSourInterval.y + vDestInterval.y;
		}
		else if (fZ >= fX && fZ >= fY)
		{
			vDir = XMVectorSet(0.f, 0.f, vDir.m128_f32[2], 0.f);
			fLength = vSourInterval.z + vDestInterval.z;
		}

		fLength -= XMVectorGetX(XMVector3Length(vDir));
		vDir = XMVector3Normalize(vDir) * fLength;
		// 충돌된 방향만 갖고온 extents만큼 빼주기.
		_vector vLine = {};
		if (true == m_pNavigationCom->isMove(vCollisionPos + vDir, &vLine))
			pCollisionObject->Set_Position(vCollisionPos + vDir);
		else
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vDir);
	}

}

void CMonster::Be_Damaged(_float fDamage, _fvector vAttackPos)
{
	m_fDamagedDelay = 30.f;
}

HRESULT CMonster::Ready_Components(_int iStartCellIndex)
{
	CNavigation::NAVIGATION_DESC desc{};
	desc.iCurrentIndex = iStartCellIndex;

	/* FOR.Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &desc)))
		return E_FAIL;

	/* FOR.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Noise"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pNoiseTextureCom), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Add_MonsterHPBar()
{
	CUI_MonsterHPBar::UI_HPBAR_DESC BarDesc = {};
	BarDesc.fRotationPerSec = XMConvertToRadians(90.f);
	BarDesc.fSpeedPerSec = 1.f;
	BarDesc.fSizeX = 101.f;
	BarDesc.fSizeY = 8.f;
	BarDesc.fX = 0.f;
	BarDesc.fY = 0.f;
	BarDesc.pMonster = this;
	
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_MonsterHPBar"), &BarDesc)))
		return E_FAIL;

	return S_OK;
}


void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pPlayerTransformCom);
	Safe_Release(m_pNoiseTextureCom);

}
