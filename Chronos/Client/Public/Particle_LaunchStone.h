#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)
class CParticle_LaunchStone final : public CGameObject
{
public:
	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos = {};
		_float3 vPivot = {};
	}PARTICLE_LAUNCHSTONE_DESC;

private:
	CParticle_LaunchStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_LaunchStone(const CParticle_LaunchStone& Prototype);
	virtual ~CParticle_LaunchStone() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };

private:
	_float4 m_vColor = {};
	_float3 m_vPivot = {};
	_float m_fSpeed = { 0.f };

private:
	HRESULT Ready_Components();

public:
	static CParticle_LaunchStone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
