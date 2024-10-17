#include "stdafx.h"
#include "Player_Body.h"
#include "GameInstance.h"

CPlayer_Body::CPlayer_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Part{pDevice, pContext}
{
}

CPlayer_Body::CPlayer_Body(const CPlayer_Body& Prototype)
	: CPlayer_Part{Prototype}
{
}

HRESULT CPlayer_Body::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CPlayer_Body::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	PLAYER_BODY_DESC* pDesc = static_cast<PLAYER_BODY_DESC*>(pArg);

	m_pPlayerTransformCom = pDesc->pPlayerTransformCom;
	Safe_AddRef(m_pPlayerTransformCom);
	m_pNavigationCom = pDesc->pNavigationCom;
	Safe_AddRef(m_pNavigationCom);
	m_pNoiseTextureCom = pDesc->pNoiseTextureCom;
	Safe_AddRef(m_pNoiseTextureCom);

	m_pRatio = pDesc->pRatio;

	m_fStartSpeed = *m_pSpeed;

	return S_OK;
}

_uint CPlayer_Body::Priority_Update(_float fTimeDelta)
{
	return OBJ_NOEVENT;
	
}

void CPlayer_Body::Update(_float fTimeDelta)
{
	m_pModelCom->SetUp_Animation(*m_pPlayerAnim, Animation_Loop(), Animation_NonInterpolate());

	Play_Animation(fTimeDelta * (*m_pSpeed / m_fStartSpeed));
	*m_pFrameIndex = Get_FrameIndex();

	StepSound();
	m_ePreAnim = *m_pPlayerAnim;
}

void CPlayer_Body::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CPlayer_Body::Render()
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
		if (FAILED(m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
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

HRESULT CPlayer_Body::Render_LightDepth()
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

void CPlayer_Body::Play_Animation(_float fTimeDelta)
{
	_vector vStateChange{};
	
	*m_pIsFinished = m_pModelCom->Play_Animation(fTimeDelta, vStateChange);

	vStateChange = m_pPlayerTransformCom->Get_Rotated_Vector(vStateChange);
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vMovePosition = vPlayerPos + vStateChange;

	_vector vLine = {};

	if (nullptr == m_pNavigationCom || true == m_pNavigationCom->isMove(vMovePosition, &vLine))
	{
		m_pPlayerTransformCom->Set_State(CTransform::STATE_POSITION, vMovePosition);
	}
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
		vMovePosition = vPlayerPos + vStateChange;

		if (true == m_pNavigationCom->isMove(vMovePosition, &vLine))
			m_pPlayerTransformCom->Set_State(CTransform::STATE_POSITION, vMovePosition);
	}
}

_vector CPlayer_Body::Get_Rotation(_matrix WorldMatrix, _vector vExist)
{
	_vector vScale, vRotationQuat, vTranslation;
	XMMatrixDecompose(&vScale, &vRotationQuat, &vTranslation, WorldMatrix);

	// 1번 방법
	// 회전 행렬 생성 (쿼터니언을 회전 행렬로 변환)
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(vRotationQuat);
	// 회전 행렬을 벡터 v에 적용
	return -1 * XMVector3TransformNormal(vExist, rotationMatrix);
}

void CPlayer_Body::StepSound()
{
	_uint iFrame[4] = {};

	switch (*m_pPlayerAnim)
	{
	case PLAYER_MOVE_JOG_F:
	case PLAYER_MOVE_JOG_B:
	case PLAYER_MOVE_JOG_L:
	case PLAYER_MOVE_JOG_R:
		iFrame[0] = 10;
		iFrame[1] = 22;
		iFrame[2] = 34;
		iFrame[3] = 46;
		break;
	case PLAYER_MOVE_WALK_F :
	case PLAYER_MOVE_WALK_B :
	case PLAYER_MOVE_WALK_L :
	case PLAYER_MOVE_WALK_R :
	case PLAYER_BLOCK_WALK_F :
	case PLAYER_BLOCK_WALK_B :
	case PLAYER_BLOCK_WALK_L :
	case PLAYER_BLOCK_WALK_R :
		iFrame[0] = 11;
		iFrame[1] = 28;
		iFrame[2] = 45;
		iFrame[3] = 62;
		break;
	case PLAYER_MOVE_DODGE_F:
		iFrame[0] = 1;
		iFrame[1] = 13;
		iFrame[2] = 24;
		iFrame[3] = 30;
		break;

	case PLAYER_MOVE_DODGE_B:
		iFrame[0] = 4;
		iFrame[1] = 12;
		iFrame[2] = 19;
		iFrame[3] = 28;
		break;

	case PLAYER_MOVE_DODGE_L:
		iFrame[0] = 2;
		iFrame[1] = 10;
		iFrame[2] = 21;
		iFrame[3] = 27;
		break;

	case PLAYER_MOVE_DODGE_R:
		iFrame[0] = 3;
		iFrame[1] = 17;
		iFrame[2] = 22;
		iFrame[3] = 30;
		break;
	}

	if (0 == iFrame[0] && 0 == iFrame[1] && 0 == iFrame[2] && 0 == iFrame[3])
	{
		m_bLeftStep = false;
		m_bRightStep = false;
		return;
	}

	if(m_ePreAnim !=*m_pPlayerAnim)
	{
		m_bLeftStep = false;
		m_bRightStep = false;
	}

	if (false == m_bLeftStep && iFrame[0] <= *m_pFrameIndex && *m_pFrameIndex < iFrame[1])
	{
		m_pGameInstance->StopSound(SOUND_PLAYER_STEP);
		m_pGameInstance->SoundPlay(TEXT("Imp_Footfall_Boot_Medium_Stone_01.ogg"), SOUND_PLAYER_STEP, 0.25f);
		m_bLeftStep = true;
		m_bRightStep = false;
	}
	else if (false == m_bRightStep && iFrame[1] <= *m_pFrameIndex && *m_pFrameIndex < iFrame[2])
	{
		m_pGameInstance->StopSound(SOUND_PLAYER_STEP);
		m_pGameInstance->SoundPlay(TEXT("Imp_Footfall_Boot_Medium_Stone_02.ogg"), SOUND_PLAYER_STEP, 0.25f);
		m_bLeftStep = false;
		m_bRightStep = true;
	}
	else if (false == m_bLeftStep && iFrame[2] <= *m_pFrameIndex && *m_pFrameIndex < iFrame[3])
	{
		m_pGameInstance->StopSound(SOUND_PLAYER_STEP);
		m_pGameInstance->SoundPlay(TEXT("Imp_Footfall_Boot_Medium_Stone_01.ogg"), SOUND_PLAYER_STEP, 0.25f);
		m_bLeftStep = true;
		m_bRightStep = false;
	}
	else if (false == m_bRightStep && iFrame[3] <= *m_pFrameIndex)
	{
		m_pGameInstance->StopSound(SOUND_PLAYER_STEP);
		m_pGameInstance->SoundPlay(TEXT("Imp_Footfall_Boot_Medium_Stone_02.ogg"), SOUND_PLAYER_STEP, 0.25f);
		m_bLeftStep = false;
		m_bRightStep = true;
	}
}

_uint CPlayer_Body::Get_FrameIndex()
{
	return m_pModelCom->Get_KeyFrameIndex();
}

_uint CPlayer_Body::Get_CurrentAnimIndex()
{
	return m_pModelCom->Get_CuttenrAnimIndex();
}

const _float4x4* CPlayer_Body::Get_BoneMatrix_Ptr(const _char* pBoneName) const
{
	return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(pBoneName);
}

HRESULT CPlayer_Body::Ready_Components()
{    
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CModel::MODEL_DESC desc = {};
	desc.m_iStartAnim = PLAYER_MOVE_IDLE;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), &desc)))
		return E_FAIL;

	return S_OK;
}

_bool CPlayer_Body::Animation_Loop()
{
	if (PLAYER_BLOCK_WALK_B == *m_pPlayerAnim ||
		PLAYER_BLOCK_WALK_F == *m_pPlayerAnim ||
		PLAYER_BLOCK_WALK_L == *m_pPlayerAnim ||
		PLAYER_BLOCK_WALK_R == *m_pPlayerAnim ||
		PLAYER_BLOCK_IDLE == *m_pPlayerAnim ||
		PLAYER_MOVE_IDLE == *m_pPlayerAnim ||
		PLAYER_MOVE_JOG_B == *m_pPlayerAnim ||
		PLAYER_MOVE_JOG_F == *m_pPlayerAnim ||
		PLAYER_MOVE_JOG_L == *m_pPlayerAnim ||
		PLAYER_MOVE_JOG_R == *m_pPlayerAnim ||
		PLAYER_MOVE_WALK_B == *m_pPlayerAnim ||
		PLAYER_MOVE_WALK_F == *m_pPlayerAnim ||
		PLAYER_MOVE_WALK_L == *m_pPlayerAnim ||
		PLAYER_MOVE_WALK_R == *m_pPlayerAnim ||
		PLAYER_JUMP_FALL == *m_pPlayerAnim ||
		PLAYER_ACTION_TELEPORT_LONG_IDLE == *m_pPlayerAnim
		)
		return true;

	return false;
}

_bool CPlayer_Body::Animation_NonInterpolate()
{
	if( PLAYER_IMPACT_HEAVY_FROMB == *m_pPlayerAnim ||
		PLAYER_IMPACT_HEAVY_FROMF == *m_pPlayerAnim ||
		PLAYER_BLOCK_IMPACT == *m_pPlayerAnim ||
		PLAYER_BLOCK_IMPACT_BREAK == *m_pPlayerAnim ||
		PLAYER_ATK_POWER_01 == *m_pPlayerAnim ||
		PLAYER_ATK_POWER_02 == *m_pPlayerAnim)
		return true;

	return false;
}

CPlayer_Body* CPlayer_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Body* pInstance = new CPlayer_Body(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_Body"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Body::Clone(void* pArg)
{

	CPlayer_Body* pInstance = new CPlayer_Body(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Create Failed : CPlayer_Body"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Body::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pPlayerTransformCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pNoiseTextureCom);
}
