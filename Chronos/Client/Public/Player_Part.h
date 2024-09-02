#pragma once

#include "PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CFSM;
class CNavigation;
END

BEGIN(Client)
class CPlayer_Part abstract : public CPartObject
{
public:
	typedef struct : CPartObject::PARTOBJ_DESC
	{
		class CFSM* pFSM = { nullptr };
		PLAYER_ANIM* pPlayerAnim = { nullptr };
		_float* pSpeed = { nullptr };
		_bool* pIsFinished = { false };
		_uint* pFrameIndex = { nullptr };
	}PLAYER_PART_DESC;

protected:
	CPlayer_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Part(const CPlayer_Part& Prototype);
	virtual ~CPlayer_Part() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	class CFSM* m_pFSM = { nullptr };

	// 상태와 공유해야 하는 변수.
	PLAYER_ANIM* m_pPlayerAnim = { nullptr };
	_float* m_pSpeed = { nullptr };
	_bool* m_pIsFinished = { false };
	_uint* m_pFrameIndex = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};
END