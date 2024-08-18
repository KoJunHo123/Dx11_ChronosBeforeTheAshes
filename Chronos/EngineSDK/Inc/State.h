#pragma once

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CState abstract: public CBase
{
public:
	typedef struct
	{
		class CFSM* pFSM = { nullptr };
	}STATE_DESC;

protected:
	CState();
	virtual ~CState() = default;

public:
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT StartState(void** pArg);
	virtual void	Priority_Update(_float fTimeDelta);
	virtual void	Update(_float fTimeDelta);
	virtual void	Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT ExitState(void** pArg);

protected:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CFSM* m_pFSM = { nullptr };

public:
	virtual void Free() override;
};
END
