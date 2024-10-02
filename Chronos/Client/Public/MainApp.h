#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT	Initialize();
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	TCHAR      m_szFPS[32];
	_int         m_iFPS;
	_float      m_fFrameTime = 0.f;

private:	
	HRESULT Ready_Prototype_Component_Static();
	HRESULT Ready_Prototype_GameObject_Static();
	HRESULT Open_Level(LEVELID eStartLevelID);
	HRESULT FrameOut(_float fTimeDelta);

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END