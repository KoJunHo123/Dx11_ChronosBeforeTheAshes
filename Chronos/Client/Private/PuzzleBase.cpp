#include "stdafx.h"
#include "PuzzleBase.h"
#include "GameInstance.h"

#include "PuzzlePart.h"
#include"Cell.h"
#include "FloorChunk.h"


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
	for (auto& pPartObject : m_Parts)
	{
		if (nullptr == pPartObject)
			continue;
		pPartObject->Update(fTimeDelta);

		CPuzzlePart* pPart = static_cast<CPuzzlePart*>(pPartObject);

		if (eCurrentLocation == pPart->Get_Location())
		{
			PuzzlePart_Cell_Active(pPart, iPlayerCellIndex);
		}
	}

	Set_NearCellActive(iSetIndex);
	Set_NearCellActive(iPlayerCellIndex);

	auto& FloorChunkList = m_pGameInstance->Get_GameObjects(LEVEL_GAMEPLAY, TEXT("Layer_FloorChunk"));

	for (auto& FloorChunk : FloorChunkList)
	{
		CFloorChunk* pFloorChunk = static_cast<CFloorChunk*>(FloorChunk);
		if (false == m_pNavigationCom->Get_CellActive(pFloorChunk->Get_CellIndex()))
		{
			pFloorChunk->Set_Dead();
		}
	}


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

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	m_pNavigationCom->Render();
#endif

	return S_OK;
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

	return S_OK;
}

void CPuzzleBase::Instead_Picking()
{
	if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_1))
	{
		// 픽셀 피킹 배우면 피킹된 좌표를 기준으로 LOCATION 찾아서 그걸로 할 예정.
		CPuzzlePart* pPart = Find_Part(LEFT_TOP);
		if (nullptr == pPart)
			return;
		
		_uint iRow = LEFT_TOP / 3;
		_uint iColumn = LEFT_TOP % 3;
		LOCATION eLocation = (LOCATION)pPart->Get_Location();
		Compare_EmplaceLocation(eLocation, pPart, iRow, iColumn);
		pPart->Set_Location(eLocation, m_PuzzlePartPoses[eLocation]);
	}
	else if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_2))
	{
		CPuzzlePart* pPart = Find_Part(MIDDLE_TOP);
		if (nullptr == pPart)
			return;

		_uint iRow = MIDDLE_TOP / 3;
		_uint iColumn = MIDDLE_TOP % 3;
		LOCATION eLocation = (LOCATION)pPart->Get_Location();
		Compare_EmplaceLocation(eLocation, pPart, iRow, iColumn);
		pPart->Set_Location(eLocation, m_PuzzlePartPoses[eLocation]);
	}
	else if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_3))
	{
		CPuzzlePart* pPart = Find_Part(RIGHT_TOP);
		if (nullptr == pPart)
			return;

		_uint iRow = RIGHT_TOP / 3;
		_uint iColumn = RIGHT_TOP % 3;
		LOCATION eLocation = (LOCATION)pPart->Get_Location();
		Compare_EmplaceLocation(eLocation, pPart, iRow, iColumn);
		pPart->Set_Location(eLocation, m_PuzzlePartPoses[eLocation]);
	}
	else if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_4))
	{
		CPuzzlePart* pPart = Find_Part(LEFT_MIDDLE);
		if (nullptr == pPart)
			return;

		_uint iRow = LEFT_MIDDLE / 3;
		_uint iColumn = LEFT_MIDDLE % 3;
		LOCATION eLocation = (LOCATION)pPart->Get_Location();
		Compare_EmplaceLocation(eLocation, pPart, iRow, iColumn);
		pPart->Set_Location(eLocation, m_PuzzlePartPoses[eLocation]);
	}
	else if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_5))
	{
		CPuzzlePart* pPart = Find_Part(MIDDLE_MIDDLE);
		if (nullptr == pPart)
			return;

		_uint iRow = MIDDLE_MIDDLE / 3;
		_uint iColumn = MIDDLE_MIDDLE % 3;
		LOCATION eLocation = (LOCATION)pPart->Get_Location();
		Compare_EmplaceLocation(eLocation, pPart, iRow, iColumn);
		pPart->Set_Location(eLocation, m_PuzzlePartPoses[eLocation]);
	}
	else if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_6))
	{
		CPuzzlePart* pPart = Find_Part(RIGHT_MIDDLE);
		if (nullptr == pPart)
			return;

		_uint iRow = RIGHT_MIDDLE / 3;
		_uint iColumn = RIGHT_MIDDLE % 3;
		LOCATION eLocation = (LOCATION)pPart->Get_Location();
		Compare_EmplaceLocation(eLocation, pPart, iRow, iColumn);
		pPart->Set_Location(eLocation, m_PuzzlePartPoses[eLocation]);
	}
	else if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_7))
	{
		CPuzzlePart* pPart = Find_Part(LEFT_DOWN);
		if (nullptr == pPart)
			return;

		_uint iRow = LEFT_DOWN / 3;
		_uint iColumn = LEFT_DOWN % 3;
		LOCATION eLocation = (LOCATION)pPart->Get_Location();
		Compare_EmplaceLocation(eLocation, pPart, iRow, iColumn);
		pPart->Set_Location(eLocation, m_PuzzlePartPoses[eLocation]);
	}
	else if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_8))
	{
		CPuzzlePart* pPart = Find_Part(MIDDLE_DOWN);
		if (nullptr == pPart)
			return;

		_uint iRow = MIDDLE_DOWN / 3;
		_uint iColumn = MIDDLE_DOWN % 3;
		LOCATION eLocation = (LOCATION)pPart->Get_Location();
		Compare_EmplaceLocation(eLocation, pPart, iRow, iColumn);
		pPart->Set_Location(eLocation, m_PuzzlePartPoses[eLocation]);
	}
	else if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_9))
	{
		CPuzzlePart* pPart = Find_Part(RIGHT_DOWN);
		if (nullptr == pPart)
			return;

		_uint iRow = RIGHT_DOWN / 3;
		_uint iColumn = RIGHT_DOWN % 3;
		LOCATION eLocation = (LOCATION)pPart->Get_Location();
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
					pCellIndices[iStateIndex] = 0;
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

void CPuzzleBase::PuzzlePart_Cell_Active(CPuzzlePart* pPart, _uint iCurrentCellIndex)
{
	_uint* pCellIndices = pPart->Get_CellIndices();

	_uint iColumn = pPart->Get_Location() / 3;
	_uint iRow = pPart->Get_Location() % 3;

	_uint iStartX = iRow * 15;
	_uint iStartZ = 30 - iColumn * 15;

	_uint iStateIndex = { 0 };

	_uint iX = (iCurrentCellIndex % 90) / 2;
	_uint iZ = iCurrentCellIndex / 90;

	if(iZ >= iStartZ + 3 && iZ <= iStartZ + 11 && iX >= iStartX + 3 && iX <= iStartX + 11)
	{
		for (size_t i = iStartZ; i < iStartZ + 15; ++i)
		{
			for (size_t j = iStartX; j < iStartX + 15; ++j)
			{
				_uint iIndex = (j * 2) + i * 90;

				if (i >= iStartZ + 3 && i <= iStartZ + 11 && j >= iStartX + 3 && j <= iStartX + 11)
				{
					m_pNavigationCom->Set_CellActive(iIndex, true);
					m_pNavigationCom->Set_CellActive(iIndex + 1, true);
					Add_FloorChunk(iIndex);
				}
				++iStateIndex;
			}
		}
	}
	else
	{
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
				++iStateIndex;
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
}
