#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point_Instance;
class CTexture;
END

BEGIN(Client)
class CWayPoint_Effect final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		
	}EFFECT_DESC;

private:
	CWayPoint_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWayPoint_Effect(const CWayPoint_Effect& Prototype);
	virtual ~CWayPoint_Effect() = default;

public:
	void Set_On(_bool bOn)
	{
		m_bOn = bOn;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };

	_bool m_bOn = { false };

private:
	HRESULT Ready_Components();

public:
	static CWayPoint_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
