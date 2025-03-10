#include "stdafx.h"
#include "Boss_Lab_Body.h"
#include "GameInstance.h"

#include "Boss_Lab.h"

#include "Camera_Container.h"
#include "Camera_Shorder.h"


CBoss_Lab_Body::CBoss_Lab_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CBoss_Lab_Body::CBoss_Lab_Body(const CBoss_Lab_Body& Prototype)
	: CPartObject(Prototype)
{
}

const _float4x4* CBoss_Lab_Body::Get_BoneMatrix_Ptr(const _char* pBoneName) const
{
	return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(pBoneName);
}

_uint CBoss_Lab_Body::Get_FrameIndex()
{
	return m_pModelCom->Get_KeyFrameIndex();
}

HRESULT CBoss_Lab_Body::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Lab_Body::Initialize(void* pArg)
{
	BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);

	m_pBossTransformCom = pDesc->pBossTransformCom;
	Safe_AddRef(m_pBossTransformCom);
	m_pNavigationCom = pDesc->pNavigationCom;
	Safe_AddRef(m_pNavigationCom);
	m_pNoiseTextureCom = pDesc->pNoiseTextureCom;
	Safe_AddRef(m_pNoiseTextureCom);

	m_pState = pDesc->pState;
	m_pAnimOver = pDesc->pAnimOver;
	m_pAnimStart = pDesc->pAnimStart;

	m_pAttackActive_LH = pDesc->pAttackActive_LH;
	m_pAttackActive_RH = pDesc->pAttackActive_RH;
	m_pAttackActive_Body = pDesc->pAttackActive_Body;
	m_pRatio = pDesc->pRatio;

	m_fChargeSpeed = 50.f;

	m_eBossAnim = BOSS_LAB_TELEPORT_LAUNCH_UNDERGROUND;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_SoundDesc.fMaxDistance = DEFAULT_DISTANCE * 3.f;
	m_SoundDesc.fVolume = 1.f;

	return S_OK;
}

_uint CBoss_Lab_Body::Priority_Update(_float fTimeDelta)
{
	XMStoreFloat3(&m_SoundDesc.vPos, m_pBossTransformCom->Get_State(CTransform::STATE_POSITION));
	return OBJ_NOEVENT;
}

void CBoss_Lab_Body::Update(_float fTimeDelta)
{
	if(true == m_bIntro)
	{
		if (CBoss_Lab::STATE_IDLE == (CBoss_Lab::STATE_IDLE & *m_pState))
		{
			m_eBossAnim = BOSS_LAB_IDLE;
		}
		else if (CBoss_Lab::STATE_WALK == (CBoss_Lab::STATE_WALK & *m_pState))
		{
			m_eBossAnim = BOSS_LAB_WALKF;
		}
		else if (CBoss_Lab::STATE_DEATH == (CBoss_Lab::STATE_DEATH & *m_pState))
		{
			if(false == *m_pAnimStart)
			{
				*m_pAnimStart = true;
				m_eBossAnim = BOSS_LAB_IMPACT_DEATH;
			}

			if (true == m_isFinished)
			{
				*m_pAnimOver = true;
				*m_pAnimStart = false;
			}
		}
		else if (CBoss_Lab::STATE_RUSH == (CBoss_Lab::STATE_RUSH & *m_pState))
		{
			if(false == *m_pAnimStart)
			{
				m_eBossAnim = BOSS_LAB_ATK_RUSH;
				*m_pAnimStart = true;
				*m_pAnimOver = false;
			}

			if (30 <= m_pModelCom->Get_KeyFrameIndex() && m_pModelCom->Get_KeyFrameIndex() < 45)
			{
				if (false == *m_pAttackActive_Body)
				{
					*m_pAttackActive_Body = true;
					Play_Sound_Short();
				}
			}
			else if (45 <= m_pModelCom->Get_KeyFrameIndex() && m_pModelCom->Get_KeyFrameIndex() < 60)
			{
				*m_pAttackActive_Body = false;
			}
			else if (60 <= m_pModelCom->Get_KeyFrameIndex() && m_pModelCom->Get_KeyFrameIndex() < 70)
			{
				if(false == *m_pAttackActive_LH)
				{
					*m_pAttackActive_LH = true;
					Play_Sound_Short();
				}
			}
			else if (70 <= m_pModelCom->Get_KeyFrameIndex())
				*m_pAttackActive_LH = false;

			if (true == m_isFinished)
			{
				*m_pAnimOver = true;
				*m_pAnimStart = false;
			}
		}
		else if (CBoss_Lab::STATE_CHARGE == (CBoss_Lab::STATE_CHARGE & *m_pState))
		{
			if (false == *m_pAnimStart)
			{
				m_eBossAnim = BOSS_LAB_ATK_CHARGE_INTO;

				m_pGameInstance->SoundPlay_Additional(TEXT("SFX_Lab_Boss_Attack_Roll_Start.ogg"), m_SoundDesc);

				*m_pAnimStart = true;
				*m_pAnimOver = false;
			}

			if(BOSS_LAB_ATK_CHARGE_INTO == m_eBossAnim && 55 < m_pModelCom->Get_KeyFrameIndex())
			{
				if(false == m_pGameInstance->IsSoundPlaying(SOUND_BOSS_CHARGE))
				{
					m_pGameInstance->SoundPlay(TEXT("SFX_Lab_Boss_Attack_Roll_Loop_Mono.ogg"), SOUND_BOSS_CHARGE, m_SoundDesc);
				}
			}

			if (BOSS_LAB_ATK_CHARGE == m_eBossAnim)
			{
				m_pBossTransformCom->Go_Straight(fTimeDelta * m_fChargeSpeed, m_pNavigationCom);
			}

			if (true == m_isFinished)
			{
				if (BOSS_LAB_ATK_CHARGE_INTO == m_eBossAnim)
				{
					m_eBossAnim = BOSS_LAB_ATK_CHARGE;
					*m_pAttackActive_Body = true;
				}
				else if (BOSS_LAB_ATK_CHARGE == m_eBossAnim && 2.f < m_fChargingTime)
				{
					m_pGameInstance->StopSound(SOUND_BOSS_CHARGE);
					m_pGameInstance->SoundPlay_Additional(TEXT("SFX_Lab_Boss_Attack_Roll_End_01.ogg"), m_SoundDesc);

					m_eBossAnim = BOSS_LAB_ATK_CHARGE_OUT;
					*m_pAttackActive_Body = false;
				}
				else if (BOSS_LAB_ATK_CHARGE_OUT == m_eBossAnim)
				{
					m_eBossAnim = BOSS_LAB_IDLE;
					*m_pAnimOver = true;
					*m_pAnimStart = false;
					m_fChargingTime = 0.f;
				}
			}
			m_fChargingTime += fTimeDelta;
		}
		else if (CBoss_Lab::STATE_NEAR == (CBoss_Lab::STATE_NEAR & *m_pState))
		{
			_uint iRandomNum = (_uint)m_pGameInstance->Get_Random(0.f, 3.f);
			if (false == *m_pAnimStart)
			{
				if (0 == iRandomNum)
					m_eBossAnim = BOSS_LAB_ATK_SWIPE_L;
				else if (1 == iRandomNum)
					m_eBossAnim = BOSS_LAB_ATK_SWIPE_R;
				else if (2 == iRandomNum)
					m_eBossAnim = BOSS_LAB_ATK_DOUBLE_SWIPE;
				*m_pAnimStart = true;

				*m_pAnimOver = false;
			}

			if (BOSS_LAB_ATK_SWIPE_L == m_eBossAnim)
			{
				if (30 <= m_pModelCom->Get_KeyFrameIndex() && 37 > m_pModelCom->Get_KeyFrameIndex())
				{
					if (false == *m_pAttackActive_LH)
					{
						Play_Sound_Short();
						*m_pAttackActive_LH = true;
					}
				}
				else
					*m_pAttackActive_LH = false;
			}
			else if (BOSS_LAB_ATK_SWIPE_R == m_eBossAnim)
			{
				if (30 <= m_pModelCom->Get_KeyFrameIndex() && 37 > m_pModelCom->Get_KeyFrameIndex())	
				{
					if(false == m_pAttackActive_RH)
					{
						Play_Sound_Short();
						*m_pAttackActive_RH = true;
					}
				}
				else
					*m_pAttackActive_RH = false;
			}
			else if (BOSS_LAB_ATK_DOUBLE_SWIPE == m_eBossAnim)
			{
				if (30 <= m_pModelCom->Get_KeyFrameIndex() && 37 > m_pModelCom->Get_KeyFrameIndex())
				{
					if(false == *m_pAttackActive_RH)
					{
						Play_Sound_Short();
						*m_pAttackActive_RH = true;
					}
				}
				else if (78 <= m_pModelCom->Get_KeyFrameIndex() && 83 > m_pModelCom->Get_KeyFrameIndex())
				{
					if (false == *m_pAttackActive_LH)
					{
						Play_Sound_Short();
						*m_pAttackActive_LH = true;
					}
				}
				else
				{
					*m_pAttackActive_RH = false;
					*m_pAttackActive_LH = false;
				}
			}
			if (true == m_isFinished)
			{
				*m_pAnimOver = true;
				*m_pAnimStart = false;
			}
		}
		else if (CBoss_Lab::STATE_STUN == (CBoss_Lab::STATE_STUN & *m_pState))
		{
			if (false == *m_pAnimStart)
			{
				m_pGameInstance->SoundPlay_Additional(TEXT("Voc_Lab_Boss_Pain_02d.ogg"), m_SoundDesc);
				m_eBossAnim = BOSS_LAB_STUN;
				*m_pAnimStart = true;
				*m_pAnimOver = false;
			}
			else if (true == m_isFinished && (BOSS_LAB_STUN == m_eBossAnim || BOSS_LAB_STUN_IMPACT == m_eBossAnim))
				m_eBossAnim = BOSS_LAB_STUN_IDLE;
			else if (CBoss_Lab::STATE_IMPACT == (CBoss_Lab::STATE_IMPACT & *m_pState) && BOSS_LAB_STUN_IMPACT == m_eBossAnim)
			{
				m_pModelCom->Reset_Animation();
				*m_pState &= ~CBoss_Lab::STATE_IMPACT;
			}
			else if (CBoss_Lab::STATE_IMPACT == (CBoss_Lab::STATE_IMPACT & *m_pState) && BOSS_LAB_STUN_IDLE == m_eBossAnim)
			{
				m_eBossAnim = BOSS_LAB_STUN_IMPACT;
				*m_pState &= ~CBoss_Lab::STATE_IMPACT;
			}
			else if (5.f < m_fStunTime && (BOSS_LAB_STUN_IDLE == m_eBossAnim || BOSS_LAB_STUN_IMPACT == m_eBossAnim))
				m_eBossAnim = BOSS_LAB_STUN_WAKEUP;
			else if (true == m_isFinished && BOSS_LAB_STUN_WAKEUP == m_eBossAnim)
			{
				*m_pAnimOver = true;
				*m_pAnimStart = false;
				m_fStunTime = 0.f;
			}
			 m_fStunTime += fTimeDelta;
		}
		else if (CBoss_Lab::STATE_TELEPORT == (CBoss_Lab::STATE_TELEPORT & *m_pState))
		{
			if (false == *m_pAnimStart)
			{
				m_eBossAnim = BOSS_LAB_TELEPORT_START;
				*m_pAnimStart = true;
				*m_pAnimOver = false;
			}
			if (BOSS_LAB_TELEPORT_LAUNCH_UNDERGROUND == m_eBossAnim)
			{
				if (2.f < m_fTeleportTime)
				{
					if (m_iTeleportCount < 2)
					{
						m_eBossAnim = BOSS_LAB_TELEPORT_LAUNCH;
						++m_iTeleportCount;

						_vector vPlayerPos = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), g_strTransformTag, 0))->Get_State(CTransform::STATE_POSITION);
						m_pBossTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);

						*m_pAttackActive_Body = true;
					}
					else
					{
						m_eBossAnim = BOSS_LAB_TELEPORT_APPEAR;
					}
				}
			}

			if (true == m_isFinished)
			{
				if (BOSS_LAB_TELEPORT_START == m_eBossAnim || BOSS_LAB_TELEPORT_LAUNCH == m_eBossAnim)
				{
					m_fTeleportTime = 0.f;

					m_eBossAnim = BOSS_LAB_TELEPORT_LAUNCH_UNDERGROUND;
					*m_pAttackActive_Body = false;
					m_bDig = false;
				}
				else if (BOSS_LAB_TELEPORT_APPEAR == m_eBossAnim)
				{
					m_pNavigationCom->Set_CurrentCellIndex_ByPos(m_pBossTransformCom->Get_State(CTransform::STATE_POSITION));
					m_pBossTransformCom->Fix_OnCell(m_pNavigationCom);

					*m_pAnimOver = true;
					*m_pAnimStart = false;
					m_bLaunch = false;
					m_fTeleportTime = 0.f;
					m_iTeleportCount = 0;
				}
			}
			m_fTeleportTime += fTimeDelta;

			_uint iFrame = m_pModelCom->Get_KeyFrameIndex();
			if (BOSS_LAB_TELEPORT_START == m_eBossAnim && false == m_bDig && 10 < iFrame)
			{
				m_pGameInstance->SoundPlay_Additional(TEXT("SFX_Lab_Boss_Attack_Teleport_Dig_01.ogg"), m_SoundDesc);
				m_bDig = true;
			}

			if (BOSS_LAB_TELEPORT_LAUNCH == m_eBossAnim)
			{
				if(false == m_bLaunch && 30 <= iFrame && iFrame < 40)
				{
					SOUND_DESC desc = m_SoundDesc;
					desc.fVolume = 0.5f;

					m_pGameInstance->SoundPlay_Additional(TEXT("SFX_Lab_Boss_Jump_Whoosh_01.ogg"), m_SoundDesc);
					m_pGameInstance->SoundPlay_Additional(TEXT("Root_Dragon_Fire_End_03.ogg"), desc);
					m_bLaunch = true;
				}
				else if (true == m_bLaunch && 62 <= iFrame && iFrame < 72)
				{
					SOUND_DESC desc = m_SoundDesc;
					desc.fVolume = 0.5f;

					// m_pGameInstance->SoundPlay_Additional(TEXT("Root_Dragon_Fire_End_03.ogg"), m_SoundDesc);
					m_bLaunch = false;
				}
			}

			if (BOSS_LAB_TELEPORT_APPEAR == m_eBossAnim)
			{
				if (false == m_bLaunch && 0 <= iFrame && iFrame < 7)
				{
					SOUND_DESC desc = m_SoundDesc;
					desc.fVolume = 0.5f;

					m_pGameInstance->SoundPlay_Additional(TEXT("Root_Dragon_Fire_End_03.ogg"), desc);
					m_bLaunch = true;
				}
			}
		}
		else if (CBoss_Lab::STATE_APPEAR == (CBoss_Lab::STATE_APPEAR & *m_pState))
		{
			if (false == *m_pAnimStart)
			{
				m_eBossAnim = BOSS_LAB_TELEPORT_LAUNCH_ROAR;
				*m_pAnimStart = true;
				*m_pAnimOver = false;
			}
			if (true == m_isFinished && BOSS_LAB_TELEPORT_LAUNCH_ROAR == m_eBossAnim)
			{
				*m_pAnimOver = true;
				*m_pAnimStart = false;
				m_bLaunch = false;
			}

			_uint iFrame = m_pModelCom->Get_KeyFrameIndex();
			if (BOSS_LAB_TELEPORT_LAUNCH_ROAR == m_eBossAnim)
			{
				if(false == m_bLaunch && 31 < iFrame && iFrame < 36)
				{
					SOUND_DESC desc = m_SoundDesc;
					desc.fVolume = 0.5f;

					m_pGameInstance->SoundPlay_Additional(TEXT("SFX_Lab_Boss_Jump_Whoosh_01.ogg"), m_SoundDesc);
					m_pGameInstance->SoundPlay_Additional(TEXT("Root_Dragon_Fire_End_03.ogg"), desc);
					m_bLaunch = true;
				}
				else if (true == m_bLaunch && 93 < iFrame && iFrame < 98)
				{
					CCamera_Shorder* m_pCamera = dynamic_cast<CCamera_Shorder*>(static_cast<CCamera_Container*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), 0))->Get_PartObject(CCamera_Container::CAMERA_SHORDER));

					m_pCamera->Set_Shaking(1.5f);

					m_pGameInstance->SoundPlay_Additional(TEXT("Voc_Lab_Boss_Alert_01a.ogg"), m_SoundDesc);
					m_bLaunch = false;
				}
			}
		}
	}

	_bool bLoop = false;
	_bool bNonInterpolate = false;
	if (BOSS_LAB_WALKF == m_eBossAnim || BOSS_LAB_IDLE == m_eBossAnim || BOSS_LAB_STUN_IDLE == m_eBossAnim)
		bLoop = true;
	
	if (BOSS_LAB_ATK_CHARGE == m_eBossAnim || BOSS_LAB_ATK_CHARGE_OUT == m_eBossAnim || BOSS_LAB_STUN_IMPACT == m_eBossAnim || BOSS_LAB_IMPACT_DEATH == m_eBossAnim)
		bNonInterpolate = true;
	
	m_pModelCom->SetUp_Animation(m_eBossAnim, bLoop, bNonInterpolate);

	Play_Animation(fTimeDelta * 1.2f);

}

void CBoss_Lab_Body::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

	if(true == m_pGameInstance->isIn_Frustum_WorldSpace(XMLoadFloat4x4(&m_WorldMatrix).r[3], 10.f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
	}
}

HRESULT CBoss_Lab_Body::Render()
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
		if(FAILED(m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;
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

HRESULT CBoss_Lab_Body::Render_LightDepth()
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

HRESULT CBoss_Lab_Body::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CModel::MODEL_DESC desc = {};
	desc.m_iStartAnim = m_eBossAnim;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Boss_Lab"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &desc)))
		return E_FAIL;

	return S_OK;
}

void CBoss_Lab_Body::Play_Animation(_float fTimeDelta)
{
	_vector vStateChange{};

	m_isFinished = m_pModelCom->Play_Animation(fTimeDelta, vStateChange);

	vStateChange = m_pBossTransformCom->Get_Rotated_Vector(vStateChange);

	_vector vBossPos = m_pBossTransformCom->Get_State(CTransform::STATE_POSITION);
	
	_vector vMovePosition = vBossPos + vStateChange;

	_vector vLine = {};

	if (nullptr == m_pNavigationCom || true == m_pNavigationCom->isMove(vMovePosition, &vLine))
		m_pBossTransformCom-> Set_State(CTransform::STATE_POSITION, vMovePosition);
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
			m_pBossTransformCom->Set_State(CTransform::STATE_POSITION, vMovePosition);
	}
}

void CBoss_Lab_Body::Play_Sound_Short()
{
	_float fRandom = { 0.f };
	fRandom = m_pGameInstance->Get_Random_Normal();

	if (fRandom < 0.3f)
	{
		m_pGameInstance->SoundPlay_Additional(TEXT("SFX_Lab_Boss_Attack_Whoosh_Distored_01.ogg"), m_SoundDesc);
		m_pGameInstance->SoundPlay_Additional(TEXT("Voc_Lab_Boss_Attack_02.ogg"), m_SoundDesc);

	}
	else if (fRandom < 0.6f)
	{
		m_pGameInstance->SoundPlay_Additional(TEXT("SFX_Lab_Boss_Attack_Whoosh_Distored_02.ogg"), m_SoundDesc);
		m_pGameInstance->SoundPlay_Additional(TEXT("Voc_Lab_Boss_Attack_03.ogg"), m_SoundDesc);
	}
	else
	{
		m_pGameInstance->SoundPlay_Additional(TEXT("SFX_Lab_Boss_Attack_Whoosh_Distored_03.ogg"), m_SoundDesc);
		m_pGameInstance->SoundPlay_Additional(TEXT("Voc_Lab_Boss_Attack_04.ogg"), m_SoundDesc);
	}
}


CBoss_Lab_Body* CBoss_Lab_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Lab_Body* pInstance = new CBoss_Lab_Body(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBoss_Lab_Body"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Lab_Body::Clone(void* pArg)
{
	CBoss_Lab_Body* pInstance = new CBoss_Lab_Body(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBoss_Lab_Body"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Lab_Body::Pooling()
{
	return new CBoss_Lab_Body(*this);
}

void CBoss_Lab_Body::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pBossTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNoiseTextureCom);
}
