#include "stdafx.h"
#include "FloorChunk.h"
#include "GameInstance.h"

CFloorChunk::CFloorChunk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CFloorChunk::CFloorChunk(const CFloorChunk& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CFloorChunk::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFloorChunk::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	FLOORCHUNK_DESC* pDesc = static_cast<FLOORCHUNK_DESC*>(pArg);

	if (FAILED(Ready_Components(pDesc->strModelTag)))
		return E_FAIL;

	m_vTargetPos = pDesc->vTargetPos;
	m_iCellIndex = pDesc->iCellIndex;
	m_eInSound = pDesc->eInSound;
	m_eOutSound = pDesc->eOutSound;

	_float3 vRandomPos = { m_pGameInstance->Get_Random(-4.f, 4.f), m_pGameInstance->Get_Random(-4.f, 0.f), m_pGameInstance->Get_Random(-4.f, 4.f) };

	_vector vPos = XMLoadFloat3(&m_vTargetPos) + XMLoadFloat3(&vRandomPos);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	XMStoreFloat3(&m_vStartPos, vPos);

	m_pTransformCom->Set_Scaled(0.1f, 0.1f, 0.1f);

	m_pPlayerTransformCom = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), g_strTransformTag));
	Safe_AddRef(m_pPlayerTransformCom);

	Play_Sound_FadeIn();

    return S_OK;
}

_uint CFloorChunk::Priority_Update(_float fTimeDelta)
{
	if (true == m_bDead)
		return OBJ_DEAD;


	return OBJ_NOEVENT;
}

void CFloorChunk::Update(_float fTimeDelta)
{
	if( false == m_bDisappear)
	{
		if (false == m_IsFadeIn)
		{
			m_IsFadeIn = m_pTransformCom->MoveTo(XMLoadFloat3(&m_vTargetPos), fTimeDelta * 5.f);
				
			_vector vCurrentDir = XMLoadFloat3(&m_vTargetPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vStartDir = XMLoadFloat3(&m_vTargetPos) - XMLoadFloat3(&m_vStartPos);

			_float fCurrentLen = XMVectorGetX(XMVector3Length(vCurrentDir));
			_float fStartLen = XMVectorGetX(XMVector3Length(vStartDir));

			_float fRatio = { 0.f };

			if (false == m_bDisappear)
				fRatio = 1 - (fCurrentLen / fStartLen);

			m_pTransformCom->Set_Scaled(fRatio, fRatio, fRatio);
		}
	}
	else
	{
		if(false == m_bFadeOutSound)
		{
			Play_Sound_FadeOut();
			m_bFadeOutSound = true;
		}


		if (false == m_IsFadeOut)
		{
			m_IsFadeOut = m_pTransformCom->MoveTo(XMLoadFloat3(&m_vStartPos), fTimeDelta * 5.f);
			_vector vCurrentDir = XMLoadFloat3(&m_vStartPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vStartDir = XMLoadFloat3(&m_vTargetPos) - XMLoadFloat3(&m_vStartPos);

			_float fCurrentLen = XMVectorGetX(XMVector3Length(vCurrentDir));
			_float fStartLen = XMVectorGetX(XMVector3Length(vStartDir));

			_float fRatio = { 0.f };

			fRatio = (fCurrentLen / fStartLen);

			m_pTransformCom->Set_Scaled(fRatio, fRatio, fRatio);
		}
		else
			m_bDead = true;
	}
}

void CFloorChunk::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

}

HRESULT CFloorChunk::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	
	_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vEmissiveColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
		//	return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ComboTexture", aiTextureType_COMBO, i)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmissiveTexture", aiTextureType_EMISSIVE, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	
    return S_OK;
}

HRESULT CFloorChunk::Ready_Components(const _wstring strModelTag)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

void CFloorChunk::Play_Sound_FadeIn()
{
	_float3 vMyPos{}, vPlayerPos{};

	XMStoreFloat3(&vMyPos, Get_Position());
	XMStoreFloat3(&vPlayerPos, m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

	switch (m_eInSound)
	{
	case SOUND_FLOOR_FADEIN_1:
		if (600 < m_pGameInstance->Get_SoundPosition(m_eInSound))
			m_pGameInstance->Set_SoundPosition(m_eInSound, 525);
		else
			m_pGameInstance->SoundPlay(TEXT("SFX_Labyrinth_Floor_FadeIn_1.ogg"), m_eInSound, 0.125f, vMyPos, vPlayerPos);
		break;

	case SOUND_FLOOR_FADEIN_2:
		if (600 < m_pGameInstance->Get_SoundPosition(m_eInSound))
			m_pGameInstance->Set_SoundPosition(m_eInSound, 537);
		else
			m_pGameInstance->SoundPlay(TEXT("SFX_Labyrinth_Floor_FadeIn_2.ogg"), m_eInSound, 0.125f, vMyPos, vPlayerPos);
		break;

	case SOUND_FLOOR_FADEIN_3:
		if (600 < m_pGameInstance->Get_SoundPosition(m_eInSound))
			m_pGameInstance->Set_SoundPosition(m_eInSound, 525);
		else
			m_pGameInstance->SoundPlay(TEXT("SFX_Labyrinth_Floor_FadeIn_3.ogg"), m_eInSound, 0.125f, vMyPos, vPlayerPos);
		break;

	case SOUND_FLOOR_FADEIN_4:
		if (600 < m_pGameInstance->Get_SoundPosition(m_eInSound))
			m_pGameInstance->Set_SoundPosition(m_eInSound, 520);
		else
			m_pGameInstance->SoundPlay(TEXT("SFX_Labyrinth_Floor_FadeIn_4.ogg"), m_eInSound, 0.125f, vMyPos, vPlayerPos);
		break;

	case SOUND_FLOOR_FADEIN_5:
		if (600 < m_pGameInstance->Get_SoundPosition(m_eInSound))
			m_pGameInstance->Set_SoundPosition(m_eInSound, 523);
		else
			m_pGameInstance->SoundPlay(TEXT("SFX_Labyrinth_Floor_FadeIn_5.ogg"), m_eInSound, 0.125f, vMyPos, vPlayerPos);
		break;
	}

}

void CFloorChunk::Play_Sound_FadeOut()
{
	_float3 vMyPos{}, vPlayerPos{};

	XMStoreFloat3(&vMyPos, Get_Position());
	XMStoreFloat3(&vPlayerPos, m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

	switch (m_eOutSound)
	{
	case SOUND_FLOOR_FADEOUT_1:
		if (140 < m_pGameInstance->Get_SoundPosition(m_eOutSound))
			m_pGameInstance->Set_SoundPosition(m_eOutSound, 135);
		else
			m_pGameInstance->SoundPlay(TEXT("SFX_Labyrinth_Floor_FadeOut_1.ogg"), m_eOutSound, 0.125f, vMyPos, vPlayerPos);
		break;

	case SOUND_FLOOR_FADEOUT_2:
		if (140 < m_pGameInstance->Get_SoundPosition(m_eOutSound))
			m_pGameInstance->Set_SoundPosition(m_eOutSound, 135);
		else
			m_pGameInstance->SoundPlay(TEXT("SFX_Labyrinth_Floor_FadeOut_2.ogg"), m_eOutSound, 0.125f, vMyPos, vPlayerPos);
		break;

	case SOUND_FLOOR_FADEOUT_3:
		if (140 < m_pGameInstance->Get_SoundPosition(m_eOutSound))
			m_pGameInstance->Set_SoundPosition(m_eOutSound, 136);
		else
			m_pGameInstance->SoundPlay(TEXT("SFX_Labyrinth_Floor_FadeOut_3.ogg"), m_eOutSound, 0.125f, vMyPos, vPlayerPos);
		break;

	case SOUND_FLOOR_FADEOUT_4:
		if (140 < m_pGameInstance->Get_SoundPosition(m_eOutSound))
			m_pGameInstance->Set_SoundPosition(m_eOutSound, 136);
		else
			m_pGameInstance->SoundPlay(TEXT("SFX_Labyrinth_Floor_FadeOut_4.ogg"), m_eOutSound, 0.125f, vMyPos, vPlayerPos);
		break;

	case SOUND_FLOOR_FADEOUT_5:
		if (140 < m_pGameInstance->Get_SoundPosition(m_eOutSound))
			m_pGameInstance->Set_SoundPosition(m_eOutSound, 134);
		else
			m_pGameInstance->SoundPlay(TEXT("SFX_Labyrinth_Floor_FadeOut_5.ogg"), m_eOutSound, 0.25f, vMyPos, vPlayerPos);
		break;
	}

}

CFloorChunk* CFloorChunk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFloorChunk* pInstance = new CFloorChunk(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CFloorChunk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFloorChunk::Clone(void* pArg)
{
	CFloorChunk* pInstance = new CFloorChunk(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFloorChunk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CFloorChunk::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pPlayerTransformCom);
}
