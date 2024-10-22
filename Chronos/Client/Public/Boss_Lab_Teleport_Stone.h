#pragma once

#include "PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)
class CBoss_Lab_Teleport_Stone final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		_float		fSpeed;
		_float		fGravity;
	} BOSSTELEPORT_STONE_DESC;
private:
	CBoss_Lab_Teleport_Stone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Lab_Teleport_Stone(const CBoss_Lab_Teleport_Stone& Prototype);
	virtual ~CBoss_Lab_Teleport_Stone() = default;

public:
	void Set_On(_fvector vPos) {
		m_bOn = true;
		Set_Position(vPos);
	}

	_bool Get_Dead() {
		return m_bDead;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CTexture* m_pTextureCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };

private:
	_bool		m_bOn = { false };
	_float3		m_vPivot = {};
	_float		m_fSpeed = {};
	_float		m_fGravity = {};


private:
	HRESULT Ready_Components();

public:
	static CBoss_Lab_Teleport_Stone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};

END