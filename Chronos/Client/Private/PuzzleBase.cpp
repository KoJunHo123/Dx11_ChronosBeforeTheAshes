#include "stdafx.h"
#include "PuzzleBase.h"
#include "GameInstance.h"

#include "PuzzlePart.h"
#include"Cell.h"

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

	return S_OK;
}

void CPuzzleBase::Priority_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
	{
		if (nullptr == pPartObject)
			continue;
		pPartObject->Priority_Update(fTimeDelta);
	}
	Instead_Picking();

	// 셀 바꾸는 거.
	if (m_pGameInstance->Key_Down('0') && nullptr == m_Parts[PART_PIECE_REPLACEMENT])
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

}

void CPuzzleBase::Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
	{
		if (nullptr == pPartObject)
			continue;
		pPartObject->Update(fTimeDelta);
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
	if (true == m_pGameInstance->Key_Down('1'))
	{
		CPuzzlePart* pPart = Find_Part(LEFT_TOP);
		if (nullptr == pPart)
			return;
		
		_uint iRow = LEFT_TOP / 3;
		_uint iColumn = LEFT_TOP % 3;
		LOCATION eLocation = (LOCATION)pPart->Get_Location();
		Compare_EmplaceLocation(eLocation, pPart, iRow, iColumn);
		pPart->Set_Location(eLocation, m_PuzzlePartPoses[eLocation]);
	}
	else if (true == m_pGameInstance->Key_Down('2'))
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
	else if (true == m_pGameInstance->Key_Down('3'))
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
	else if (true == m_pGameInstance->Key_Down('4'))
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
	else if (true == m_pGameInstance->Key_Down('5'))
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
	else if (true == m_pGameInstance->Key_Down('6'))
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
	else if (true == m_pGameInstance->Key_Down('7'))
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
	else if (true == m_pGameInstance->Key_Down('8'))
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
	else if (true == m_pGameInstance->Key_Down('9'))
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

}
