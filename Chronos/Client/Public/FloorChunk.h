#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CFloorChunk final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_wstring	strModelTag;
		_float3		vTargetPos;
		_int		iCellIndex;
	} FLOORCHUNK_DESC;

private:
	CFloorChunk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFloorChunk(const CFloorChunk& Prototype);
	virtual ~CFloorChunk() = default;

public:
	_int Get_CellIndex() {
		return m_iCellIndex;
	}

	void Set_Dead()
	{
		m_bDisappear = true;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

	_float3 m_vTargetPos = {};
	_float3 m_vStartPos = {};

	_int m_iCellIndex = { -1 };

	_bool m_bDisappear = { false };

private:
	HRESULT Ready_Components(const _wstring strModelTag);

public:
	static CFloorChunk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
