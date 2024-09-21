#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	typedef struct
	{
		class CGameObject* pOwnerObject;
		void* pBoundingDesc;
		_bool bCollisionOnce = { false };

		_wstring strColliderTag;
	}COLLIDER_DESC;

	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	void Set_OnCollision(_bool bOnCollision) {
		m_bOnCollision = bOnCollision;
	}


public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(const _float4x4* pWorldMatrix);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

public:
	_bool Intersect(const _wstring strColliderTag, CCollider* pTargetCollider);

private:
	class CBounding* m_pBounding = { nullptr };
	TYPE			m_eColliderType = {};

	class CGameObject* m_pOwnerObject = { nullptr };
	_bool m_bOnCollision = { false };
	_bool m_bCollisionOnce = { false };
	_wstring m_strColliderTag = L"";

	unordered_set<CCollider*> m_CollisionSet;

	

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END