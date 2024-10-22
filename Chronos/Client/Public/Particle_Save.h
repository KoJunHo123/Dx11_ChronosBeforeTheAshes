#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point_Instance;
class CTexture;
END

BEGIN(Client)
class CParticle_Save final : public CGameObject
{
public:
	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos = {};
	}PARTICLE_SAVE_DESC;

private:
	CParticle_Save(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Save(const CParticle_Save& Prototype);
	virtual ~CParticle_Save() = default;

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
	static CParticle_Save* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};
END
