#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CPuzzlePart final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_float3 vPos;
		_float3 vScale;
		_float3 vRotation;

		_wstring strModelTag;
		_uint iCurrentLocation;

		_uint* pCellIndices;
	}PUZZLEPART_DESC;

private:
	CPuzzlePart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPuzzlePart(const CPuzzlePart& Prototype);
	virtual ~CPuzzlePart() = default;

public:
	_uint Get_Location() {
		return m_iCurrentLocation;
	}

	_vector Get_CurrentPos() {
		return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	}

	_uint* Get_CellIndices() {
		return m_CellIndices;
	}

	void Set_Location(_uint iLocation, _float3 vTargetPos) {
		m_iCurrentLocation = iLocation;
		m_vTargetPos = vTargetPos;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	void Exchange_Part(CPuzzlePart* pPart) {
		m_vTargetPos = pPart->m_vTargetPos;
	}

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

	_uint m_iCurrentLocation = { 0 };

	_float3 m_vTargetPos = {};

	_uint m_CellIndices[255] = {};

private:
	HRESULT Ready_Components(const _wstring strModelTag);

public:
	static CPuzzlePart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};
END
