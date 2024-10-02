#include "stdafx.h"
#include "PuzzleBase.h"
#include "GameInstance.h"

#include "PuzzlePart.h"
#include "Puzzle_InterColl.h"

#include"Cell.h"
#include "FloorChunk.h"

#include "Monster.h"
#include "Teleport.h"
#include "Particle_Spawn.h"

#include "Camera_Container.h"

CPuzzleBase::CPuzzleBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject(pDevice, pContext)
{
}

CPuzzleBase::CPuzzleBase(const CPuzzleBase& Prototype)
	: CContainerObject(Prototype)
{
}

HRESULT CPuzzleBase::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPuzzleBase::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PuzzlePart_Pos()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	PUZZLEBASE_DESC* pDesc = static_cast<PUZZLEBASE_DESC*>(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPos), 1.f));
	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));

	m_pPlayerNavigationCom = static_cast<CNavigation*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Navigation")));
	Safe_AddRef(m_pPlayerNavigationCom);

	m_pColliderCom->Set_OnCollision(true);

	m_pCameraContainer = static_cast<CCamera_Container*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), 0));
	Safe_AddRef(m_pCameraContainer);

	return S_OK;
}

_uint CPuzzleBase::Priority_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
	{
		if (nullptr == pPartObject)
			continue;
		pPartObject->Priority_Update(fTimeDelta);
	}

	if(CCamera_Container::CAMERA_INTERACTION == m_pCameraContainer->Get_CurrentCamera())
		Instead_Picking();

	// 파츠 바꾸는 거.
	if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_0) && nullptr == m_Parts[PART_PIECE_REPLACEMENT])
	{
		_uint CellIndices[255] = {};
		CPuzzlePart* pPart = static_cast<CPuzzlePart*>(m_Parts[PART_PIECE_11]);

		CPuzzlePart::PUZZLEPART_DESC desc{};
			
		desc.fRotationPerSec = XMConvertToRadians(90.f);
		desc.fSpeedPerSec = 1.f;
		desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();


		desc.strModelTag = TEXT("Prototype_Component_Model_Puzzle_ReplacementPiece");
		desc.iCurrentLocation = pPart->Get_Location();
		XMStoreFloat3(&desc.vPos, pPart->Get_CurrentPos());
		desc.vRotation = {};
		desc.vScale = { 1.f, 1.f, 1.f };

		Make_Road(PART_PIECE_REPLACEMENT, CellIndices);
		desc.pCellIndices = CellIndices;
		Update_Cell((LOCATION)pPart->Get_Location(), CellIndices);

		if (FAILED(__super::Add_PartObject(PART_PIECE_REPLACEMENT, TEXT("Prototype_GameObject_PuzzlePart"), &desc)))
			MSG_BOX(TEXT("바꾸기 실패..."));

		CPuzzlePart* pPartReplcae = static_cast<CPuzzlePart*>(m_Parts[PART_PIECE_REPLACEMENT]);
		pPartReplcae->Exchange_Part(pPart);

		Safe_Release(m_Parts[PART_PIECE_11]);
	}

	return OBJ_NOEVENT;
}

void CPuzzleBase::Update(_float fTimeDelta)
{
	_uint iPlayerCellIndex = m_pPlayerNavigationCom->Get_CurrentCellIndex();


	_uint iSetIndex = { 0 };
	if (0 == iPlayerCellIndex % 2)
		iSetIndex = iPlayerCellIndex + 1;
	else
		iSetIndex = iPlayerCellIndex - 1;

	Set_FallCellDeactive(iSetIndex);
	Set_FallCellDeactive(iPlayerCellIndex);

	LOCATION eCurrentLocation = Find_CurrentLocation(iPlayerCellIndex);
	_uint iPartIndex = { 0 };
	for (auto& pPartObject : m_Parts)
	{
		if (nullptr == pPartObject)
			continue;
		pPartObject->Update(fTimeDelta);

		CPuzzlePart* pPart = static_cast<CPuzzlePart*>(pPartObject);

		if (eCurrentLocation == pPart->Get_Location())
		{
			PuzzlePart_Cell_Active(pPart, iPlayerCellIndex, iPartIndex);
		}
		++iPartIndex;
	}

	Set_NearCellActive(iSetIndex);
	Set_NearCellActive(iPlayerCellIndex);

	auto& FloorChunkList = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, TEXT("Layer_FloorChunk"));

	for (auto& FloorChunk : FloorChunkList)
	{
		CFloorChunk* pFloorChunk = static_cast<CFloorChunk*>(FloorChunk);
		if (false == m_pNavigationCom->Get_CellActive(pFloorChunk->Get_CellIndex()) || false == Check_InFloor())
		{
			pFloorChunk->Set_Dead();
		}
	}
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CPuzzleBase::Late_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
	{
		if (nullptr == pPartObject)
			continue;
		pPartObject->Late_Update(fTimeDelta);
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
	m_pGameInstance->Add_DebugObject(m_pNavigationCom);
#endif

}

HRESULT CPuzzleBase::Render()
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

void CPuzzleBase::Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval)
{
	if (TEXT("Coll_Player") == strColliderTag)
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
		pCollisionObject->Set_Position(vCollisionPos + vDir);
	}

}

HRESULT CPuzzleBase::Ready_Components()
{
	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Puzzle_Base"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC BoundingDesc = {};
	BoundingDesc.vCenter = { 0.f, 0.f, 0.f };
	BoundingDesc.vExtents = { 3.f, 3.f, 3.f };
	
	CCollider::COLLIDER_DESC CollDesc = {};
	CollDesc.bCollisionOnce = false;
	CollDesc.pOwnerObject = this;
	CollDesc.strColliderTag = TEXT("Coll_Obstacle");
	CollDesc.pBoundingDesc = &BoundingDesc;

	/* FOR.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &CollDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPuzzleBase::Ready_PuzzlePart_Pos()
{
	m_PuzzlePartPoses[LEFT_TOP] = { -1.75f, 0.6f, 1.75f };
	m_PuzzlePartPoses[MIDDLE_TOP] = { 0.f, 0.6f, 1.75f };
	m_PuzzlePartPoses[RIGHT_TOP] = { 1.75f, 0.6f, 1.75f };

	m_PuzzlePartPoses[LEFT_MIDDLE] = { -1.75f, 0.6f, 0.f };
	m_PuzzlePartPoses[MIDDLE_MIDDLE] = { 0.f, 0.6f, 0.f };
	m_PuzzlePartPoses[RIGHT_MIDDLE] = { 1.75f, 0.6f, 0.f };

	m_PuzzlePartPoses[LEFT_DOWN] = { -1.75f, 0.6f, -1.75f };
	m_PuzzlePartPoses[MIDDLE_DOWN] = { 0.f, 0.6f, -1.75f };
	m_PuzzlePartPoses[RIGHT_DOWN] = { 1.75f, 0.6f, -1.75f };

	return S_OK;
}

HRESULT CPuzzleBase::Ready_Parts()
{
	/* 실제 추가하고 싶은 파트오브젝트의 갯수만큼 밸류를 셋팅해놓자. */
	m_Parts.resize(PART_END);

	CPuzzlePart::PUZZLEPART_DESC desc{};
	desc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.fSpeedPerSec = 1.f;
	desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

	// 좌상단
	desc.strModelTag = TEXT("Prototype_Component_Model_Puzzle_Piece_00");
	desc.iCurrentLocation = LEFT_TOP;
	desc.vPos = m_PuzzlePartPoses[LEFT_TOP];
	desc.vRotation = {};
	desc.vScale = { 1.f, 1.f, 1.f };

	_uint CellIndices[255] = {};
	Make_Road(PART_PIECE_00, CellIndices);
	desc.pCellIndices = CellIndices;
	Update_Cell(LEFT_TOP, CellIndices);
	if(FAILED(__super::Add_PartObject(PART_PIECE_00, TEXT("Prototype_GameObject_PuzzlePart"), &desc)))
		return E_FAIL;

	// 중상단
	desc.strModelTag = TEXT("Prototype_Component_Model_Puzzle_Piece_01");
	desc.iCurrentLocation = MIDDLE_TOP;
	desc.vPos = m_PuzzlePartPoses[MIDDLE_TOP];
	desc.vRotation = {};
	desc.vScale = { 1.f, 1.f, 1.f };

	Make_Road(PART_PIECE_01, CellIndices);
	desc.pCellIndices = CellIndices;
	Update_Cell(MIDDLE_TOP, CellIndices);
	if (FAILED(__super::Add_PartObject(PART_PIECE_01, TEXT("Prototype_GameObject_PuzzlePart"), &desc)))
		return E_FAIL;

	// 우상단
	desc.strModelTag = TEXT("Prototype_Component_Model_Puzzle_Piece_02");
	desc.iCurrentLocation = RIGHT_TOP;
	desc.vPos = m_PuzzlePartPoses[RIGHT_TOP];
	desc.vRotation = {};
	desc.vScale = { 1.f, 1.f, 1.f };

	Make_Road(PART_PIECE_02, CellIndices);
	desc.pCellIndices = CellIndices;
	Update_Cell(RIGHT_TOP, CellIndices);
	if (FAILED(__super::Add_PartObject(PART_PIECE_02, TEXT("Prototype_GameObject_PuzzlePart"), &desc)))
		return E_FAIL;

	// 좌중단
	desc.strModelTag = TEXT("Prototype_Component_Model_Puzzle_Piece_10");
	desc.iCurrentLocation = LEFT_MIDDLE;
	desc.vPos = m_PuzzlePartPoses[LEFT_MIDDLE];
	desc.vRotation = {};
	desc.vScale = { 1.f, 1.f, 1.f };

	Make_Road(PART_PIECE_10, CellIndices);
	desc.pCellIndices = CellIndices;
	Update_Cell(LEFT_MIDDLE, CellIndices);
	if (FAILED(__super::Add_PartObject(PART_PIECE_10, TEXT("Prototype_GameObject_PuzzlePart"), &desc)))
		return E_FAIL;

	// 중중단
	desc.strModelTag = TEXT("Prototype_Component_Model_Puzzle_Piece_11");
	desc.iCurrentLocation = MIDDLE_MIDDLE;
	desc.vPos = m_PuzzlePartPoses[MIDDLE_MIDDLE];
	desc.vRotation = {};
	desc.vScale = { 1.f, 1.f, 1.f };

	Make_Road(PART_PIECE_11, CellIndices);
	desc.pCellIndices = CellIndices;
	Update_Cell(MIDDLE_MIDDLE, CellIndices);
	if (FAILED(__super::Add_PartObject(PART_PIECE_11, TEXT("Prototype_GameObject_PuzzlePart"), &desc)))
		return E_FAIL;

	// 우중단
	desc.strModelTag = TEXT("Prototype_Component_Model_Puzzle_Piece_12");
	desc.iCurrentLocation = RIGHT_MIDDLE;
	desc.vPos = m_PuzzlePartPoses[RIGHT_MIDDLE];
	desc.vRotation = {};
	desc.vScale = { 1.f, 1.f, 1.f };

	Make_Road(PART_PIECE_12, CellIndices);
	desc.pCellIndices = CellIndices;
	Update_Cell(RIGHT_MIDDLE, CellIndices);
	if (FAILED(__super::Add_PartObject(PART_PIECE_12, TEXT("Prototype_GameObject_PuzzlePart"), &desc)))
		return E_FAIL;

	// 중하단
	desc.strModelTag = TEXT("Prototype_Component_Model_Puzzle_Piece_21");
	desc.iCurrentLocation = MIDDLE_DOWN;
	desc.vPos = m_PuzzlePartPoses[MIDDLE_DOWN];
	desc.vRotation = {};
	desc.vScale = { 1.f, 1.f, 1.f };

	Make_Road(PART_PIECE_21, CellIndices);
	desc.pCellIndices = CellIndices;
	Update_Cell(MIDDLE_DOWN, CellIndices);
	if (FAILED(__super::Add_PartObject(PART_PIECE_21, TEXT("Prototype_GameObject_PuzzlePart"), &desc)))
		return E_FAIL;

	// 우하단
	desc.strModelTag = TEXT("Prototype_Component_Model_Puzzle_Piece_22");
	desc.iCurrentLocation = RIGHT_DOWN;
	desc.vPos = m_PuzzlePartPoses[RIGHT_DOWN];
	desc.vRotation = {};
	desc.vScale = { 1.f, 1.f, 1.f };

	Make_Road(PART_PIECE_22, CellIndices);
	desc.pCellIndices = CellIndices;
	Update_Cell(RIGHT_DOWN, CellIndices);
	if (FAILED(__super::Add_PartObject(PART_PIECE_22, TEXT("Prototype_GameObject_PuzzlePart"), &desc)))
		return E_FAIL;

	fill(CellIndices, CellIndices + 255, 1);
	Update_Cell(m_eEmplaceLocation, CellIndices);

	CPuzzle_InterColl::PART_INTERCOLL_DESC InterDesc = {};
	InterDesc.fRotationPerSec = 0.f;
	InterDesc.fSpeedPerSec = 0.f;
	InterDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

	if (FAILED(__super::Add_PartObject(PART_INTERACTION_COLL, TEXT("Prototype_GameObject_Puzzle_InterColl"), &InterDesc)))
		return E_FAIL;

	return S_OK;
}

void CPuzzleBase::Instead_Picking()
{
	_float3 vPickPos = {};
	if (m_pGameInstance->Get_DIMouseState_Down(DIMK_LBUTTON) && true == m_pGameInstance->Picking(&vPickPos))
	{
		// 좌표로 로케이션 찾는 함수.
		// 그 로케이션으로 밑에거 삭 해주면 끝.
		LOCATION eLocation = Find_Location_ByPos(XMLoadFloat3(&vPickPos));

		CPuzzlePart* pPart = Find_Part(eLocation);
		if (nullptr == pPart)
			return;

		_uint iRow = eLocation / 3;
		_uint iColumn = eLocation % 3;
		//eLocation = (LOCATION)pPart->Get_Location();
		Compare_EmplaceLocation(eLocation, pPart, iRow, iColumn);
		pPart->Set_Location(eLocation, m_PuzzlePartPoses[eLocation]);
	}
}

CPuzzlePart* CPuzzleBase::Find_Part(LOCATION eLocation)
{
	for (auto& Part : m_Parts)
	{
		if (nullptr == Part)
			continue;

		CPuzzlePart* pPart = static_cast<CPuzzlePart*>(Part);

		if (eLocation == pPart->Get_Location())
		{
			return pPart;
		}
	}
	return nullptr;
}

void CPuzzleBase::Compare_EmplaceLocation(LOCATION& eLocation, class CPuzzlePart* pPart, _uint iRow, _uint iColumn)
{
	switch (iRow)
	{
	case 0:
		if (m_eEmplaceLocation == eLocation + 3)
		{
			LOCATION eChange = eLocation;
			eLocation = m_eEmplaceLocation;
			m_eEmplaceLocation = eChange;

			_uint iEmptyCellState[255] = {};
			fill(iEmptyCellState, iEmptyCellState + 255, 1);
			Update_Cell(m_eEmplaceLocation, iEmptyCellState);
			Update_Cell(eLocation, pPart->Get_CellIndices());
		}
		break;
	case 1:
		if (m_eEmplaceLocation == eLocation - 3)
		{
			LOCATION eChange = eLocation;
			eLocation = m_eEmplaceLocation;
			m_eEmplaceLocation = eChange;

			_uint iEmptyCellState[255] = {};
			fill(iEmptyCellState, iEmptyCellState + 255, 1);
			Update_Cell(m_eEmplaceLocation, iEmptyCellState);
			Update_Cell(eLocation, pPart->Get_CellIndices());
		}
		else if (m_eEmplaceLocation == eLocation + 3)
		{
			LOCATION eChange = eLocation;
			eLocation = m_eEmplaceLocation;
			m_eEmplaceLocation = eChange;

			_uint iEmptyCellState[255] = {};
			fill(iEmptyCellState, iEmptyCellState + 255, 1);
			Update_Cell(m_eEmplaceLocation, iEmptyCellState);
			Update_Cell(eLocation, pPart->Get_CellIndices());
		}
		break;
	case 2:
		if (m_eEmplaceLocation == eLocation - 3)
		{
			LOCATION eChange = eLocation;
			eLocation = m_eEmplaceLocation;
			m_eEmplaceLocation = eChange;
			
			_uint iEmptyCellState[255] = {};
			fill(iEmptyCellState, iEmptyCellState + 255, 1);
			Update_Cell(m_eEmplaceLocation, iEmptyCellState);
			Update_Cell(eLocation, pPart->Get_CellIndices());
		}
		break;
	deafult:
		break;
	}

	switch (iColumn)
	{
	case 0:
		if (m_eEmplaceLocation == eLocation + 1)
		{
			LOCATION eChange = eLocation;
			eLocation = m_eEmplaceLocation;
			m_eEmplaceLocation = eChange;

			_uint iEmptyCellState[255] = {};
			fill(iEmptyCellState, iEmptyCellState + 255, 1);
			Update_Cell(m_eEmplaceLocation, iEmptyCellState);
			Update_Cell(eLocation, pPart->Get_CellIndices());
		}
		break;
	case 1:
		if (m_eEmplaceLocation == eLocation - 1)
		{
			LOCATION eChange = eLocation;
			eLocation = m_eEmplaceLocation;
			m_eEmplaceLocation = eChange;

			_uint iEmptyCellState[255] = {};
			fill(iEmptyCellState, iEmptyCellState + 255, 1);
			Update_Cell(m_eEmplaceLocation, iEmptyCellState);
			Update_Cell(eLocation, pPart->Get_CellIndices());
		}
		else if (m_eEmplaceLocation == eLocation + 1)
		{
			LOCATION eChange = eLocation;
			eLocation = m_eEmplaceLocation;
			m_eEmplaceLocation = eChange;

			_uint iEmptyCellState[255] = {};
			fill(iEmptyCellState, iEmptyCellState + 255, 1);
			Update_Cell(m_eEmplaceLocation, iEmptyCellState);
			Update_Cell(eLocation, pPart->Get_CellIndices());
		}
		break;
	case 2:
		if (m_eEmplaceLocation == eLocation - 1)
		{
			LOCATION eChange = eLocation;
			eLocation = m_eEmplaceLocation;
			m_eEmplaceLocation = eChange;

			_uint iEmptyCellState[255] = {};
			fill(iEmptyCellState, iEmptyCellState + 255, 1);
			Update_Cell(m_eEmplaceLocation, iEmptyCellState);
			Update_Cell(eLocation, pPart->Get_CellIndices());
		}
		break;
	default:
		break;
	}

}

void CPuzzleBase::Make_Road(PART ePart, _uint* pCellIndices)
{
	_uint iStateIndex = { 0 };

	for (size_t z = 0; z < 15; ++z)
	{
		for (size_t x = 0; x < 15; ++x)
		{
			switch (ePart)
			{
			case PART_PIECE_00:
				if (z == 4 && x >= 0 && x <= 7)
					pCellIndices[iStateIndex] = 0;
				else if (x == 7 && z >= 4 && z <= 10)
					pCellIndices[iStateIndex] = 0;
				else if (z == 10 && x >= 7 && x <= 14)
					pCellIndices[iStateIndex] = 0;
				else
					pCellIndices[iStateIndex] = 1;
				break;

			case PART_PIECE_01:
				if (x == 4 && z >= 4 && z <= 14)
					pCellIndices[iStateIndex] = 0;
				else if (z == 4 && x >= 4 && x <= 14)
					pCellIndices[iStateIndex] = 0;
				else
					pCellIndices[iStateIndex] = 1;
				break;

			case PART_PIECE_02:
				if (x == 10 && z >= 0 && z <= 2)
					pCellIndices[iStateIndex] = 0;
				else
					pCellIndices[iStateIndex] = 1;
				break;

			case PART_PIECE_10:
				if (x == 4 && z >= 0 && z <= 4)
					pCellIndices[iStateIndex] = 0;
				else if(z == 4 && x >=4 && x <= 14)
					pCellIndices[iStateIndex] = 0;
				else
					pCellIndices[iStateIndex] = 1;
				break;

			case PART_PIECE_11:
				pCellIndices[iStateIndex] = 1;
				break;

			case PART_PIECE_12:
				if (x == 4 && z >= 0 && z <= 10)
					pCellIndices[iStateIndex] = 0;
				else if(z == 10 && x >= 4 && x <= 14)
					pCellIndices[iStateIndex] = 0;
				else
					pCellIndices[iStateIndex] = 1;
				break;

			case PART_PIECE_21:
				if(x == 4 && z >= 0 && z <=10)
					pCellIndices[iStateIndex] = 0;
				else if(z == 10 && x>= 0 && x <= 4)
					pCellIndices[iStateIndex] = 0;
				else 
					pCellIndices[iStateIndex] = 1;
				break;

			case PART_PIECE_22:
				if(z == 10 && x >= 0 && x <= 4)
					pCellIndices[iStateIndex] = 0;
				else if(x == 4 && z >= 10 && z <= 14)
					pCellIndices[iStateIndex] = 0;
				else
					pCellIndices[iStateIndex] = 1;
				break;

			case PART_PIECE_REPLACEMENT:
				if (z == 4 && x >= 0 && x <= 10)
					pCellIndices[iStateIndex] = 0;
				else if (x == 10 && z >= 4 && z <= 14)
					pCellIndices[iStateIndex] = 0;
				else
					pCellIndices[iStateIndex] = 1;
				break;

			default:
				break;
			}

			if(PART_PIECE_11 != ePart)
			{
				if (x >= 3 && x <= 11 && z >= 3 && z <= 11)
				{
					if (PART_PIECE_02 == ePart)
					{
						if ((3 <= x && x <= 4) || (10 <= x && x <= 11) || (3 <= z && z <= 4) || (10 <= z && z <= 11))
							pCellIndices[iStateIndex] = 0;
						else if((6 <= x && x <= 8) && (6 <= z && z <= 8))
							pCellIndices[iStateIndex] = 0;
						else if(7 == x && 5 == z)
							pCellIndices[iStateIndex] = 0;
					}
					else
						pCellIndices[iStateIndex] = 0;
				}
			}

			++iStateIndex;
		}
	}

}

void CPuzzleBase::Update_Cell(LOCATION eLocation, _uint* pCellStates)
{
	_uint iColumn = eLocation / 3;
	_uint iRow = eLocation % 3;

	_uint iStartX = iRow * 15;
	_uint iStartZ = 30 - iColumn * 15;

	_uint iStateIndex = { 0 };

	for (size_t i = iStartZ; i < iStartZ + 15; ++i) 
	{
		for (size_t j = iStartX; j < iStartX + 15; ++j)
		{
			_uint iIndex = (j * 2) + i * 90;

			m_pNavigationCom->Set_CellType(iIndex, pCellStates[iStateIndex]);
			m_pNavigationCom->Set_CellType(iIndex + 1, pCellStates[iStateIndex]);

			++iStateIndex;
		}
	}
}

void CPuzzleBase::PuzzlePart_Cell_Active(CPuzzlePart* pPart, _uint iCurrentCellIndex, _uint iPartIndex)
{
	_uint iColumn = pPart->Get_Location() / 3;
	_uint iRow = pPart->Get_Location() % 3;

	_uint iStartX = iRow * 15;
	_uint iStartZ = 30 - iColumn * 15;

	_uint iX = (iCurrentCellIndex % 90) / 2;
	_uint iZ = iCurrentCellIndex / 90;

	if(iZ >= iStartZ + 3 && iZ <= iStartZ + 11 && iX >= iStartX + 3 && iX <= iStartX + 11)
	{
		if (PART_PIECE_02 == iPartIndex)
		{
			if (0 == m_pGameInstance->Get_ObjectSize(LEVEL_GAMEPLAY, TEXT("Layer_Teleport")))
			{
				_uint iTeleportIndex = ((iStartX + 7) * 2) + (iStartZ + 7) * 90;
				if (1 == iTeleportIndex % 2)
					--iTeleportIndex;
				Add_Teleport(XMLoadFloat3(&m_pNavigationCom->Get_CellZXCenter(iTeleportIndex)));
			}
		}

		for (size_t i = iStartZ; i < iStartZ + 15; ++i)
		{
			for (size_t j = iStartX; j < iStartX + 15; ++j)
			{
				_uint iIndex = (j * 2) + i * 90;

				if (i >= iStartZ + 3 && i <= iStartZ + 11 && j >= iStartX + 3 && j <= iStartX + 11)
				{
					if(0 == m_pNavigationCom->Get_CellType(iIndex))
					{
						m_pNavigationCom->Set_CellActive(iIndex, true);
						m_pNavigationCom->Set_CellActive(iIndex + 1, true);
						Add_FloorChunk(iIndex);
					}
				}
			}
		}
		// 여기서 몬스터 배치.
		// Add_Monster(iStartX + 3, iStartX + 11, iStartZ + 3, iStartZ + 11, iPartIndex);
	}
	else
	{
		if (PART_PIECE_02 == iPartIndex)
		{
			m_pGameInstance->Clear_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Teleport"));
		}
		for (size_t i = iStartZ; i < iStartZ + 15; ++i)
		{
			for (size_t j = iStartX; j < iStartX + 15; ++j)
			{
				_uint iIndex = (j * 2) + i * 90;

				if (i >= iStartZ + 3 && i <= iStartZ + 11 && j >= iStartX + 3 && j <= iStartX + 11)
				{
					m_pNavigationCom->Set_CellActive(iIndex, false);
					m_pNavigationCom->Set_CellActive(iIndex + 1, false);
				}
			}
		}
	}
	
	
}

CPuzzleBase::LOCATION CPuzzleBase::Find_CurrentLocation(_uint iCellIndex)
{
	_uint iX = (iCellIndex % 90) / 2;
	iX += (iCellIndex % 90) % 2;
	_uint iZ = iCellIndex / 90;

	LOCATION eCurrentLocation = { LOCATION_END };

	if (iX < 15 && iZ < 15)
		eCurrentLocation = LEFT_DOWN;
	else if (iX < 30 && iZ < 15)
		eCurrentLocation = MIDDLE_DOWN;
	else if (iX < 45 && iZ < 15)
		eCurrentLocation = RIGHT_DOWN;
	else if (iX < 15 && iZ < 30)
		eCurrentLocation = LEFT_MIDDLE;
	else if (iX < 30 && iZ < 30)
		eCurrentLocation = MIDDLE_MIDDLE;
	else if (iX < 45 && iZ < 30)
		eCurrentLocation = RIGHT_MIDDLE;
	else if (iX < 15 && iZ < 45)
		eCurrentLocation = LEFT_TOP;
	else if (iX < 30 && iZ < 45)
		eCurrentLocation = MIDDLE_TOP;
	else if (iX < 45 && iZ < 45)
		eCurrentLocation = RIGHT_TOP;

	return eCurrentLocation;
}

void CPuzzleBase::Set_NearCellActive(_uint iCellIndex, _uint iCount)
{
	if (1 < iCount)
		return;

	_float3 vNearCells = m_pNavigationCom->Get_NearCellIndex(iCellIndex);

	_int iAB = (_int)vNearCells.x;
	_int iBC = (_int)vNearCells.y;
	_int iCA = (_int)vNearCells.z;

	if (1 == iAB % 2)
		iAB -= 1;
	if (1 == iBC % 2)
		iBC -= 1;
	if (1 == iCA % 2)
		iCA -= 1;

	if (iAB < m_iPuzzleCellIndex && CCell::TYPE_WALK == m_pNavigationCom->Get_CellType(iAB))
	{
		m_pNavigationCom->Set_CellActive(iAB, true);
		m_pNavigationCom->Set_CellActive(iAB + 1, true);
		Add_FloorChunk(iAB);

		//Set_NearCellActive(iAB, iCount + 1);
		//Set_NearCellActive(iAB + 1, iCount + 1);
	}

	if (iBC < m_iPuzzleCellIndex && CCell::TYPE_WALK == m_pNavigationCom->Get_CellType(iBC))
	{
		m_pNavigationCom->Set_CellActive(iBC, true);
		m_pNavigationCom->Set_CellActive(iBC + 1, true);
		Add_FloorChunk(iBC);

		//Set_NearCellActive(iBC, iCount + 1);
		//Set_NearCellActive(iBC + 1, iCount + 1);
	}

	if (iCA < m_iPuzzleCellIndex && CCell::TYPE_WALK == m_pNavigationCom->Get_CellType(iCA))
	{
		m_pNavigationCom->Set_CellActive(iCA, true);
		m_pNavigationCom->Set_CellActive(iCA + 1, true);
		Add_FloorChunk(iCA);

		//Set_NearCellActive(iCA, iCount + 1);
		//Set_NearCellActive(iCA + 1, iCount + 1);
	}
}

void CPuzzleBase::Set_FallCellDeactive(_uint iCellIndex, _uint iCount)
{
	if (1 < iCount)
		return;

	_float3 vNearCells = m_pNavigationCom->Get_NearCellIndex(iCellIndex);

	_int iAB = (_int)vNearCells.x;
	_int iBC = (_int)vNearCells.y;
	_int iCA = (_int)vNearCells.z;

	if (1 == iAB % 2)
		iAB -= 1;
	if (1 == iBC % 2)
		iBC -= 1;
	if (1 == iCA % 2)
		iCA -= 1;

	if (iAB < m_iPuzzleCellIndex && CCell::TYPE_WALK == m_pNavigationCom->Get_CellType(iAB))
	{
		
		m_pNavigationCom->Set_CellActive(iAB, false);
		m_pNavigationCom->Set_CellActive(iAB + 1, false);

		Set_FallCellDeactive(iAB, iCount + 1);
		Set_FallCellDeactive(iAB + 1, iCount + 1);
	}

	if (iBC < m_iPuzzleCellIndex && CCell::TYPE_WALK == m_pNavigationCom->Get_CellType(iBC))
	{
		m_pNavigationCom->Set_CellActive(iBC, false);
		m_pNavigationCom->Set_CellActive(iBC + 1, false);

		Set_FallCellDeactive(iBC, iCount + 1);
		Set_FallCellDeactive(iBC + 1, iCount + 1);
	}

	if (iCA < m_iPuzzleCellIndex && CCell::TYPE_WALK == m_pNavigationCom->Get_CellType(iCA))
	{
		m_pNavigationCom->Set_CellActive(iCA, false);
		m_pNavigationCom->Set_CellActive(iCA + 1, false);

		Set_FallCellDeactive(iCA, iCount + 1);
		Set_FallCellDeactive(iCA + 1, iCount + 1);
	}

	if(5723 == iCellIndex || 5724 == iCellIndex)
	{
		m_pNavigationCom->Set_CellActive(4092, false);
		m_pNavigationCom->Set_CellActive(4093, false);
	}

}

HRESULT CPuzzleBase::Add_FloorChunk(_int iCellIndex)
{
	if (0 != iCellIndex % 2)
		iCellIndex -= 1;
	
	auto& FloorChunkList = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, TEXT("Layer_FloorChunk"));

	for (auto& FloorChunk : FloorChunkList)
	{
		CFloorChunk* pFloorChunk = static_cast<CFloorChunk*>(FloorChunk);
		if (iCellIndex == pFloorChunk->Get_CellIndex())
			return S_OK;
	}

	CFloorChunk::FLOORCHUNK_DESC desc = {};
	desc.fRotationPerSec = 0.f;
	desc.fSpeedPerSec = 0.f;
	desc.iCellIndex = iCellIndex;
	desc.vTargetPos = m_pNavigationCom->Get_CellZXCenter(iCellIndex);

	desc.strModelTag = TEXT("Prototype_Component_Model_FloorChunk_A");
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FloorChunk"), TEXT("Prototype_GameObject_FloorChunk"), &desc)))
		return E_FAIL;

	desc.strModelTag = TEXT("Prototype_Component_Model_FloorChunk_B");
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FloorChunk"), TEXT("Prototype_GameObject_FloorChunk"), &desc)))
		return E_FAIL;

	desc.strModelTag = TEXT("Prototype_Component_Model_FloorChunk_C");
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FloorChunk"), TEXT("Prototype_GameObject_FloorChunk"), &desc)))
		return E_FAIL;

	desc.strModelTag = TEXT("Prototype_Component_Model_FloorChunk_D");
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FloorChunk"), TEXT("Prototype_GameObject_FloorChunk"), &desc)))
		return E_FAIL;

	desc.strModelTag = TEXT("Prototype_Component_Model_FloorChunk_E");
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FloorChunk"), TEXT("Prototype_GameObject_FloorChunk"), &desc)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CPuzzleBase::Add_Monster(_uint iStartX, _uint iEndX, _uint iStartZ, _uint iEndZ, _uint iPartIndex)
{
	// 요 범위 내에서 현재 파트 인덱스에 따라서 몬스터 할당.
	vector<_uint> AddIndices;

	_int iStartCellIndex = { 0 };


	if (PART_PIECE_00 == iPartIndex && false == m_bPartActive[PART_PIECE_00])
	{
		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_DRUM, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_MAGE, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_CONSTRUCT, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_TROLL, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_TROLL, 1.5f)))
		//	return E_FAIL;

		m_bPartActive[PART_PIECE_00] = true;
	}
	else if (PART_PIECE_01 == iPartIndex && false == m_bPartActive[PART_PIECE_01])
	{
		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_DRUM, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_DRUM, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_DRUM, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_CONSTRUCT, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_CONSTRUCT, 1.5f)))
		//	return E_FAIL;

		m_bPartActive[PART_PIECE_01] = true;
	}
	else if (PART_PIECE_02 == iPartIndex && false == m_bPartActive[PART_PIECE_02])
	{
		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_MAGE, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_MAGE, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_MAGE, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_TROLL, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_TROLL, 1.5f)))
		//	return E_FAIL;

		m_bPartActive[PART_PIECE_02] = true;
	}
	else if (PART_PIECE_10 == iPartIndex && false == m_bPartActive[PART_PIECE_10])
	{
		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_CONSTRUCT, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_CONSTRUCT, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_CONSTRUCT, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_CONSTRUCT, 1.5f)))
		//	return E_FAIL;

		m_bPartActive[PART_PIECE_10] = true;
	}
	else if (PART_PIECE_12 == iPartIndex && false == m_bPartActive[PART_PIECE_12])
	{
		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_DRUM, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_MAGE, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_MAGE, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_CONSTRUCT, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_CONSTRUCT, 1.5f)))
		//	return E_FAIL;

		m_bPartActive[PART_PIECE_12] = true;
	}
	else if (PART_PIECE_21 == iPartIndex && false == m_bPartActive[PART_PIECE_21])
	{
		for(size_t i = 0; i < 8; ++i)
		{
			//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
			//if(FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_TROLL, 1.5f)))
			//	return E_FAIL;
			//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Lab_Troll"), &desc)))
			//	return E_FAIL;
		}

		m_bPartActive[PART_PIECE_21] = true;
	}
	else if (PART_PIECE_22 == iPartIndex && false == m_bPartActive[PART_PIECE_22])
	{
		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_TROLL, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_TROLL, 1.5f)))
		//	return E_FAIL;
		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_TROLL, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_TROLL, 1.5f)))
		//	return E_FAIL;
		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_CONSTRUCT, 1.5f)))
		//	return E_FAIL;

		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_CONSTRUCT, 1.5f)))
		//	return E_FAIL;

		m_bPartActive[PART_PIECE_22] = true;
	}

	else if (PART_PIECE_REPLACEMENT == iPartIndex && false == m_bPartActive[PART_PIECE_REPLACEMENT])
	{
		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_DRUM, 1.5f)))
		//	return E_FAIL;
		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_DRUM, 1.5f)))
		//	return E_FAIL;
		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_DRUM, 1.5f)))
		//	return E_FAIL;
		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_DRUM, 1.5f)))
		//	return E_FAIL;
		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_MAGE, 1.5f)))
		//	return E_FAIL;
		//iStartCellIndex = Get_DiffIndex(AddIndices, iStartX, iEndX, iStartZ, iEndZ);
		//if (FAILED(Add_SpawnParticle(iStartCellIndex, CParticle_Spawn::TYPE_MAGE, 1.5f)))
		//	return E_FAIL;

		m_bPartActive[PART_PIECE_REPLACEMENT] = true;
		}

	return S_OK;
}

_uint CPuzzleBase::Get_DiffIndex(vector<_uint>& AddIndices, _uint iStartX, _uint iEndX, _uint iStartZ, _uint iEndZ)
{
	_uint iIndex = 0;

	while (true)
	{
		_uint iRandomX = (_uint)m_pGameInstance->Get_Random((_float)iStartX, (_float)iEndX);
		_uint iRandomZ = (_uint)m_pGameInstance->Get_Random((_float)iStartZ, (_float)iEndZ);

		iIndex = (iRandomX * 2) + iRandomZ * 90;
		_bool isSame = { false };
		for (auto& AddIndex : AddIndices)
		{
			if (iIndex == AddIndex)
				isSame = true;
		}

		if (false == isSame && 0 == m_pNavigationCom->Get_CellType(iIndex))
			break;
	}
	AddIndices.push_back(iIndex);

	return iIndex;
}

HRESULT CPuzzleBase::Add_Teleport(_fvector vPos)
{
	CTransform* pTransform = static_cast<CTransform*>(m_pGameInstance->Find_PartComponent(LEVEL_GAMEPLAY, TEXT("Layer_Interaction"), g_strTransformTag, 1, 6));

	CTeleport::TELEPORT_DESC desc = { };
	desc.fRotationPerSec = 0.f;
	desc.fSpeedPerSec = 1.f;
	desc.pParentWorldMatrix = nullptr;
	XMStoreFloat3(&desc.vPos, vPos);
	XMStoreFloat3(&desc.vTeleportPos, pTransform->Get_State(CTransform::STATE_POSITION));
	desc.vColor = _float4(0.541f, 0.169f, 0.886f, 1.f);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Teleport"), TEXT("Prototype_GameObject_Teleport"), &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPuzzleBase::Add_SpawnParticle(_int iSpawnCellIndex, _uint iType, _float fOffset)
{
	CParticle_Spawn::PARTICLE_SPAWN_DESC desc = {};

	desc.fRotationPerSec = 0.f;
	desc.fSpeedPerSec = 1.f;
	desc.iSpawnCellIndex = iSpawnCellIndex;
	XMStoreFloat3(&desc.vPos, m_pNavigationCom->Get_CellCenterPos(desc.iSpawnCellIndex));
	desc.vPos.y += fOffset;

	if (-1 == desc.iSpawnCellIndex)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Spawn"), &desc)))
		return E_FAIL;

	return S_OK;

	return S_OK;
}

CPuzzleBase::LOCATION CPuzzleBase::Find_Location_ByPos(_fvector vPos)
{
	_float fSize = m_PuzzlePartPoses[RIGHT_MIDDLE].x * 0.5f;

	_float3 vLocalPos = {};

	XMStoreFloat3(&vLocalPos, XMVector3TransformCoord(vPos, m_pTransformCom->Get_WorldMatrix_Inverse()));

	for (size_t i = 0; i < LOCATION_END; ++i)
	{
		if (m_PuzzlePartPoses[i].x - fSize <= vLocalPos.x && vLocalPos.x <= m_PuzzlePartPoses[i].x + fSize &&
			m_PuzzlePartPoses[i].z - fSize <= vLocalPos.z && vLocalPos.z <= m_PuzzlePartPoses[i].z + fSize)
		{
			return (LOCATION)i;
		}
	}

	return LOCATION_END;
}

_bool CPuzzleBase::Check_InFloor()
{
	_int iPlayerIndex = m_pPlayerNavigationCom->Get_CurrentCellIndex();
	if(m_iPuzzleCellIndex <= iPlayerIndex || (6089 != iPlayerIndex && 6090 != iPlayerIndex && 6091 != iPlayerIndex)
		|| (6049 != iPlayerIndex && 6050 != iPlayerIndex) || (6082 != iPlayerIndex && 6083 != iPlayerIndex))
		return true;

	return false;
}

CPuzzleBase* CPuzzleBase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPuzzleBase* pInstance = new CPuzzleBase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CPuzzleBase"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPuzzleBase::Clone(void* pArg)
{
	CPuzzleBase* pInstance = new CPuzzleBase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CPuzzleBase"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPuzzleBase::Free()
{
	__super::Free();
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pPlayerNavigationCom);
	Safe_Release(m_pColliderCom);

	Safe_Release(m_pCameraContainer);
}
