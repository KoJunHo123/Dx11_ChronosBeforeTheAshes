#pragma once

#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
class CNavigation;
END


BEGIN(Client)
class CParticle_Spawn final : public CBlendObject
{
public:
	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos = {};
		_int iSpawnCellIndex = { 0 };
	}PARTICLE_SPAWN_DESC;
private:
	CParticle_Spawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Spawn(const CParticle_Spawn& Prototype);
	virtual ~CParticle_Spawn() = default;

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
	class CVIBuffer_Point_Instance* m_pVIBufferCom_Converge = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom_Spread = { nullptr };

private:
	_float3 m_vPivot = {};

	_bool m_bSpawn = { false };
	_float m_fSpeed = { 0.f };
	_int m_iSpawnCellIndex = { 0 };

	_bool m_bConvergeOver = { false };
	_bool m_bSpread = { false };

	_float m_fSpawnTime = { 0.f };

private:
	HRESULT Ready_Components();
	HRESULT Add_Monster();
	HRESULT Add_ParticleSmoke();

public:
	static CParticle_Spawn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END
