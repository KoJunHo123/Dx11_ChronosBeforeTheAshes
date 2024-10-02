#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	/* IDirect3DDevice9 == ID3D11Device + ID3D11DeviceContext */
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize(_uint iLevelIndex);
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	_uint Get_LevelIndex() {
		return m_iLevelIndex;
	}

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

	_uint m_iLevelIndex = { 0 };

public:
	virtual void Free() override;
};

END