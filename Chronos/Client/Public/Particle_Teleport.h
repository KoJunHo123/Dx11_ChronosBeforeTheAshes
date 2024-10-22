#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point_Instance;
class CTexture;
END

BEGIN(Client)
class CParticle_Teleport final : public CGameObject
{
public:
	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos = {};
	}PARTICLE_TELEPORT_DESC;

private:
	CParticle_Teleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Teleport(const CParticle_Teleport& Prototype);
	virtual ~CParticle_Teleport() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CParticle_Teleport* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;

};
END
