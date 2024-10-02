#pragma once

#include "Base.h"
#include "Transform.h"

/* 모든 게임내에 사용되는 게임 오브젝트들의 부모 클래스다. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		
	} GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	_bool Get_Dead() {
		return m_bDead;
	}

	void Set_Dead() {
		m_bDead = true;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_LightDepth() { return S_OK; }

public:
	class CComponent* Find_Component(const _wstring& strComponentTag);
	virtual class CComponent* Find_PartComponent(const _wstring& strComponentTag, _uint iPartObjIndex);

	virtual void Intersect(const _wstring strColliderTag, CGameObject* pCollisionObject, _float3 vSourInterval, _float3 vDestInterval);
	virtual _vector Get_Position();
	virtual void Set_Position(_vector vPos);

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CTransform*			m_pTransformCom = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	_bool	m_isCloned = { false };
	_bool	m_bDead = { false };

protected:
	map<const _wstring, class CComponent*>			m_Components;

protected:
	
	HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END