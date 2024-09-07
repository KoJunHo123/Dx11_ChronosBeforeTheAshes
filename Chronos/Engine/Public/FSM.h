#pragma once

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CFSM : public CComponent
{
private:
	CFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFSM(const CFSM& Prototype);
	virtual ~CFSM() = default;

public:
	HRESULT Add_State(class CState* pState);
	void Set_State(_uint iNextState) {
		m_iNextStateIndex = iNextState;
	}

	_uint Get_State() {
		return m_iNextStateIndex;
	}

	CState* Get_State(_uint iIndex) {
		return m_States[iIndex];
	}


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	HRESULT Render();

private:
	HRESULT Change_State();
	
private:
	_uint m_iCurrentStateIndex = { 0 };
	_uint m_iNextStateIndex = { 0 };
	vector<class CState*> m_States;

public:
	static CFSM* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFSM* Clone(void* pArg) override;
	virtual void Free();
};
END
