#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CNavigation;
class CShader;
class CModel;
END

BEGIN(Client)
class CPuzzleBase final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos;
		_float3 vScale;
		_float3 vRotation;
	}PUZZLEBASE_DESC;

	enum PART { PART_PIECE_00, PART_PIECE_01, PART_PIECE_02, PART_PIECE_10, PART_PIECE_11, PART_PIECE_12, PART_PIECE_21, PART_PIECE_22, PART_END };

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

	virtual HRESULT Save_Data(ofstream* pOutFile) override;

private:
	class CNavigation* m_pNavigationCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

	PUZZLEBASE_DESC m_Desc = {};

private:
	HRESULT Ready_Components();

public:
	static CPuzzleBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
