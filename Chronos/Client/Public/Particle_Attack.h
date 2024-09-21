#pragma once

#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
END


BEGIN(Client)
class CParticle_Attack final : public CBlendObject
{
public:
	enum TYPE { TYPE_PLAYER, TYPE_MONSTER, TYPE_END };

	typedef struct : CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos = {};
		_float3 vPivot = {};

		TYPE eType = {};
	}PARTICLE_ATTACK_DESC;

private:
	CParticle_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle_Attack(const CParticle_Attack& Prototype);
	virtual ~CParticle_Attack() = default;

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
	_float3 m_vPivot = {};

private:
	HRESULT Ready_Components(TYPE eType);

public:
	static CParticle_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END
