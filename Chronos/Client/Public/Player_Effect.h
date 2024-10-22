#pragma once

#include "PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point_Instance;
class CTexture;
END

BEGIN(Client)
class CPlayer_Effect final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		
	}EFFECT_DESC;
private:
	CPlayer_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Effect(const CPlayer_Effect& Prototype);
	virtual ~CPlayer_Effect() = default;

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

private:
	_float2 m_vDivide = {};

private:
	HRESULT Ready_Components();

public:
	static CPlayer_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};
END
