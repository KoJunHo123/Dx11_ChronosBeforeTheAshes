#pragma once

#include "Base.h"

BEGIN(Engine)
class CColliderLayer final : public CBase
{
private:
	CColliderLayer();
	virtual ~CColliderLayer() = default;

public:
	HRESULT Add_Collider(class CCollider* pCollider);
	HRESULT Update(_wstring strDestCollider, _wstring strSourCollider, CColliderLayer* pColliderLayer);

private:
	list<class CCollider*>			m_Colliders;

public:
	static CColliderLayer* Create();
	virtual void Free() override;
};
END
