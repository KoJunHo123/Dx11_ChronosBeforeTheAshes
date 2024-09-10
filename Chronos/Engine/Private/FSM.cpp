#include "FSM.h"
#include "State.h"

_uint CFSM::m_iCurrentStateIndex = { 0 };

CFSM::CFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CFSM::CFSM(const CFSM& Prototype)
	: CComponent(Prototype)
{
}

HRESULT CFSM::Add_State(CState* pState)
{
	if (nullptr == pState)
		return E_FAIL;

	m_States.emplace_back(pState);

	return S_OK;
}

HRESULT CFSM::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFSM::Initialize(void* pArg)
{

	return S_OK;
}

void CFSM::Priority_Update(_float fTimeDelta)
{
	if(FAILED(Change_State()))
	{
		MSG_BOX(TEXT("State Change Failed"));
		return;
	}

	if(nullptr != m_States[m_iCurrentStateIndex])
		m_States[m_iCurrentStateIndex]->Priority_Update(fTimeDelta);
}

void CFSM::Update(_float fTimeDelta)
{
	if (nullptr != m_States[m_iCurrentStateIndex])
		m_States[m_iCurrentStateIndex]->Update(fTimeDelta);
}

void CFSM::Late_Update(_float fTimeDelta)
{
	if (nullptr != m_States[m_iCurrentStateIndex])
		m_States[m_iCurrentStateIndex]->Late_Update(fTimeDelta);
}

HRESULT CFSM::Render()
{
	if (nullptr != m_States[m_iCurrentStateIndex])
	{
		if (FAILED(m_States[m_iCurrentStateIndex]->Render()))
			return E_FAIL;
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CFSM::Change_State()
{
	if(m_iCurrentStateIndex != m_iNextStateIndex)
	{
		void* pArg = { nullptr };

		if (FAILED(m_States[m_iCurrentStateIndex]->ExitState(&pArg)))
			return E_FAIL;

		if (FAILED(m_States[m_iNextStateIndex]->StartState(&pArg)))
			return E_FAIL;

		m_iCurrentStateIndex = m_iNextStateIndex;
	}

	return S_OK;
}


CFSM* CFSM::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFSM* pInstance = new CFSM(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CFSM"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CFSM* CFSM::Clone(void* pArg)
{
	CFSM* pInstance = new CFSM(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CFSM"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFSM::Free()
{
	__super::Free();
	for (auto& State : m_States)
		Safe_Release(State);
}
