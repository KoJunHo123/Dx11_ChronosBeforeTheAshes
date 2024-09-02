#pragma once

#include "Client_Defines.h"
#include "ContainerObject.h"

BEGIN(Engine)
class CNavigation;
class CShader;
class CModel;
END

BEGIN(Client)
class CPuzzleBase final : public CContainerObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos;
		_float3 vScale;
		_float3 vRotation;
	}PUZZLEBASE_DESC;

	enum PART { PART_PIECE_00, PART_PIECE_01, PART_PIECE_02, PART_PIECE_10, PART_PIECE_11, PART_PIECE_12, PART_PIECE_21, PART_PIECE_22, PART_PIECE_REPLACEMENT, PART_END };
	enum LOCATION { LEFT_TOP, MIDDLE_TOP, RIGHT_TOP, LEFT_MIDDLE, MIDDLE_MIDDLE, RIGHT_MIDDLE, LEFT_DOWN, MIDDLE_DOWN, RIGHT_DOWN, LOCATION_END };

private:
	CPuzzleBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPuzzleBase(const CPuzzleBase& Prototype);
	virtual ~CPuzzleBase() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CNavigation* m_pNavigationCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

	_float3 m_PuzzlePartPoses[LOCATION_END] = {};
	LOCATION m_eEmplaceLocation = { LEFT_DOWN };

private:
	HRESULT Ready_Components();
	HRESULT Ready_PuzzlePart_Pos();
	HRESULT Ready_Parts();

	void Instead_Picking();
	class CPuzzlePart* Find_Part(LOCATION eLocation);
	void Compare_EmplaceLocation(LOCATION& eLocation, class CPuzzlePart* pPart, _uint iRow, _uint iColumn);

	void Make_Road(PART ePart, _uint* pCellIndices);
	void Update_Cell(LOCATION eLocation, _uint* pCellStates);

public:
	static CPuzzleBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
